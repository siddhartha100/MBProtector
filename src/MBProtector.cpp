/* This tool is used to put BWLOCK dynamically in a program binary */
#include "pin.H"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string.h>
#include <iomanip>
#include <unistd.h>
#include <ctime>
#include <sys/syscall.h>
#include "bwlock.h"
#define SYS_bwlock 323
#include<time.h>

#define MAX_FUNCTION_TO_BE_PROTECTED 34000
using namespace std;


// Holds instruction count for a single procedure
typedef struct RtnCount
{
    string _name;                       // Routine name
    string _image;                      // Image name
    string _address;                    // Routine address
    UINT64 _rtnCount;                   // Number of times routine is called
    UINT64 _icount;                     // Instruction count of the routine
    UINT64 _memicount;                  // Memory instruction count of the routine
    double _percentMemoryUsage;         // Memory instruction percentage of the routine
    struct RtnCount * _next;
} RTN_COUNT;

// Linked list of instruction counts for each routine
RTN_COUNT * RtnList = 0;

//temporary variables
string procedure;
string image;
string address;
UINT64 calls;
UINT64 instructions;
UINT64 memInstructions;
UINT64 percent;
UINT64 percentOfMemoryInstruction;
string functionName;
UINT64 noOfBwlockCalled;
UINT64 noOfBwUnlockCalled;
int    noOfFunctionsToBeProtected;
const char *functionNameInC;
RTN currentRoutine;
time_t start;
time_t stop;
string functionsToBeInstrumented[MAX_FUNCTION_TO_BE_PROTECTED];
string inputFunction;
const char *inputFunctionNameInC;

// Instrument function, called each time a memory critical section is found
void bwlock(){
	syscall(SYS_bwlock, 0, 1);             // BWLOCK system call 
	noOfBwlockCalled++;                    // Number of times BWLOCK is called during execution time
}

// Instrument function, called each time a memory critical section is found
void bwunlock(){
	syscall(SYS_bwlock, 0, 0);             // BWUNLOCK system call 
	noOfBwUnlockCalled++;                  // Number of times BWUNLOCK is called during execution time
}

/* Instrumentation routines */
VOID ImageLoad(IMG img, VOID *v)
{   
    for(int i=0;i<noOfFunctionsToBeProtected;i++)
	{
		inputFunction = functionsToBeInstrumented[i];
		inputFunctionNameInC = inputFunction.c_str();
		for (RTN_COUNT * rc1 = RtnList ; rc1; rc1 = rc1->_next)
		{	
			functionName = rc1->_name;
			functionNameInC = functionName.c_str();
				  
			//if ( ((rc1->_rtnCount > 40000) && (rc1->_percentMemoryUsage > 10)) || rc1->_memicount > 6000000) {
			if (  strcmp(functionNameInC,inputFunctionNameInC) == 0)
			{		     
				currentRoutine = RTN_FindByName(img, functionNameInC);
				if (RTN_Valid(currentRoutine))
				{
					RTN_Open(currentRoutine);
					RTN_InsertCall(currentRoutine, IPOINT_BEFORE,(AFUNPTR)bwlock, IARG_END);     // Applying BWLOCK 
					RTN_Close(currentRoutine);

					RTN_Open( currentRoutine);
					RTN_InsertCall(currentRoutine, IPOINT_AFTER, (AFUNPTR)bwunlock, IARG_END);   // Applying BWUNLOCK
					RTN_Close(currentRoutine );	
				}		
			}
		}
	
	}
}
       
// This function is called when the application exits
VOID Fini(INT32 code, VOID *v)
{
    cout << "done" << endl;
	
	// Number of times BWLOCK called during execution period
    cout << "noOfBwlockCalled = " << noOfBwlockCalled << endl;
    time(&stop);
	
	// Total program execution time
    printf("Execution time: %f",difftime(stop,start));    
}

/* Print Help Message */                                                  
INT32 Usage()
{
    cerr << "This Pintool counts the number of times a routine is executed" << endl;
    cerr << "and the number of instructions executed in a routine" << endl;
    return -1;
}

/* Main function */                                                              
int main(int argc, char * argv[])
{       
    // Initialize pin & symbol manager
    PIN_InitSymbols();
	if (PIN_Init(argc, argv)) return Usage();
    
	// Reading profiling information 
	string profilingInfoFileNameWithPath;
    const char *fileNameWithPath;
    cout << "Please provide the name(with path) of the file that contains profiling information " << endl;
    cin >> profilingInfoFileNameWithPath;
    fileNameWithPath = profilingInfoFileNameWithPath.c_str();
    std::ifstream file(fileNameWithPath);
        
    while (file >> procedure >> image >> address >> calls >> instructions >> memInstructions >> percent)
    {
		// Allocate a counter for this routine
		RTN_COUNT * rc = new RTN_COUNT;
	 
		// The RTN goes away when the image is unloaded, so save it now because we need it in the fini
		rc->_name = procedure;
		rc->_image = image;
		rc->_address = address;
		rc->_icount = instructions;
		rc->_rtnCount = calls;
		rc->_memicount = memInstructions;
		rc->_percentMemoryUsage = ((rc->_memicount)*100)/(rc->_icount) ;
		rc->_next = RtnList;
		RtnList = rc;
	}
    file.close();
	
	// Taking functions to be instrumented from the user
	cout << "\n Provide how many function you want to instrument? " << endl;
	cin >> noOfFunctionsToBeProtected;
	string temp;
	int i=0;
	for(i=0;i<noOfFunctionsToBeProtected;i++){
		cout << "Give " << i <<"th function name: " << endl;
		cin >> temp;
		functionsToBeInstrumented[i] = temp;
	}
    	
    // Register ImageLoad to be called to instrument instructions
    IMG_AddInstrumentFunction(ImageLoad, 0);
   
    // This function will be called when application exits
    PIN_AddFiniFunction(Fini, 0);
       
    // Start the program, never returns
    time(&start);             //In order to find tool overhead
    PIN_StartProgram();
    
    return 0;
}


    
