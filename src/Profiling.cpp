/* This Pin tool is used for profiling a program binary*/
#include "pin.H"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string.h>
#include <iomanip>
#include <unistd.h>

using namespace std;

// Program profiling information will be stored in a text file
ofstream outFile;
KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool","o", "proccount.out", "specify output file name");

// Holds instruction count for a single procedure
typedef struct RtnCount
{
    string _name;                       // Routine name
    string _image;                      // Image name
    ADDRINT _address;                   // Routine address
    RTN _rtn;                           
    UINT64 _rtnCount;                   // Number of times routine is called
    UINT64 _icount;                     // Instruction count of the routine
    UINT64 _memicount;                  // Memory instruction count of the routine
    struct RtnCount * _next;
} RTN_COUNT;

// Linked list of instruction counts for each routine
RTN_COUNT * RtnList = 0;

// This function is called before every instruction is executed
VOID doCount(UINT64 * counter)
{
    (*counter)++;
}

// This function is called before every memory instruction is executed
VOID doMemoryCount(UINT64 * memCounter)
{
    (*memCounter)++;
}

const char * StripPath(const char * path)
{
    const char * file = strrchr(path,'/');
    if (file)
        return file+1;
    else
        return path;
}

/* Instrumentation routines */
VOID ImageLoad(IMG img, VOID *v)
{   

    // Scanning all files in an image 
    for (SEC sec = IMG_SecHead(img); SEC_Valid(sec); sec = SEC_Next(sec))
    {    

        for (RTN rtn = SEC_RtnHead(sec); RTN_Valid(rtn); rtn = RTN_Next(rtn))
        {
               
            // Allocate a counter for this routine
            RTN_COUNT * rc = new RTN_COUNT;

            // The RTN goes away when the image is unloaded, so save it now 
            // because we need it in the fini
    		rc->_name = RTN_Name(rtn);
    		rc->_image = StripPath(IMG_Name(SEC_Img(RTN_Sec(rtn))).c_str());
    		rc->_address = RTN_Address(rtn);
    		rc->_icount = 0;
    		rc->_rtnCount = 0;
            rc->_memicount = 0;

   	        // Add to list of routines
			rc->_next = RtnList;
			RtnList = rc;

            // Open the RTN.
            RTN_Open( rtn );
            
  	        // Insert a call at the entry point of a routine to increment the call count
            RTN_InsertCall(rtn, IPOINT_BEFORE, (AFUNPTR)docount, IARG_PTR, &(rc->_rtnCount), IARG_END);
            
            // For each instruction of the routine
   	        for (INS ins = RTN_InsHead(rtn); INS_Valid(ins); ins = INS_Next(ins))
			{

       		    // Insert a call to docount to increment the instruction counter for this rtn
        		INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)doCount, IARG_PTR, &(rc->_icount), IARG_END);

                // Check whether memory read instruction 
                if (INS_IsMemoryRead(ins))
                {
                    		
					// Insert a call to do_memory_count to increment the instruction counter for this rtn
        	    	INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)doMemoryCount, IARG_PTR, &(rc->_memicount), IARG_END);
                
				}
                
                // Check whether memory write instruction 
                if (INS_IsMemoryWrite(ins))
                {
                  	
					// Insert a call to do_memory_count to increment the instruction counter for this rtn
        	   		INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)doMemoryCount, IARG_PTR, &(rc->_memicount), IARG_END);
               	
				}


			}

            // Close the RTN.
            RTN_Close( rtn );
			
        }

     	
    }


}

// This function is called when the application exits .It prints profiling information of each function
VOID Fini(INT32 code, VOID *v)
{
   
    for (RTN_COUNT * rc = RtnList; rc; rc = rc->_next)
    {
        if (rc->_icount > 0)
		{
            outFile << " " << rc->_name << " "
                  << " " << rc->_image << " "
                  << " " << hex << rc->_address << dec <<" "
                  << " " << rc->_rtnCount << " "
                  << " " << rc->_icount << " "
                  << " " << rc->_memicount << " "
                  << " " << (rc->_memicount*100/rc->_icount) << endl;
		}
		
    }

}

/* Print Help Message */                                                   
INT32 Usage()
{
    cerr << endl << KNOB_BASE::StringKnobSummary() << endl;
    return -1;
}

/* Main function */                                                                 
int main(int argc, char * argv[])
{
    // Initialize pin & symbol manager
    PIN_InitSymbols();
	
	// Open a file in order to save output
    outFile.open("proccount.out");
    if (PIN_Init(argc, argv)) return Usage();
    
	// Register ImageLoad to be called to instrument instructions
    IMG_AddInstrumentFunction(ImageLoad, 0);
   
    PIN_AddFiniFunction(Fini, 0);

    // Start the program, never returns
    PIN_StartProgram();
    
    return 0;
}

    
