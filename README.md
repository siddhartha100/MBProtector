MBProtector: A Dynamic Memory Bandwidth Protection Tool. 

This tool is able to protect memory critical sections on any program. It modifies 
the program binary in order for that.



Building and running the tool: 


1. Installing correct linux kernel

In order to use this tool, first you need to install a specific linux kernel. 
This kernel contains necessary system calls needed for this tool.You can download 
that kernel from Github.Command is - git clone https://github.com/heechul/linux -b rtas15

During installation, you need to select BWLOCK option in configuration file.


2. Installing MemGuard 

You need to install MemGuard module in Linux. Download MemGuard module from GitHub.

Commands are - 1. git clone https://github.com/heechul/memguard.git
               2. git checkout rtas15-bwlockv2

The procedure of adding MemGuard module is 
               1. make
               2. insmod memguard.ko


3. Building MBProtector tool

A. Goto MBProtector/src/pin-2.14-71313-gcc.4.4.7-linux/source/tools directory
B. Copy "MyPinTool" directory and save it as "Profiling" directory
C. Copy Profiling.cpp to Profiling directory
D. Open makefile.rules and modify "TEST_TOOL_ROOTS := MyPinTool" to TEST_TOOL_ROOTS := Profiling
E. Go to Profiling directory and run this command - make obj-intel64/Profiling.so


4. Running the tool

Run this command from Profiling directory - "../../../pin -t obj-intel64/Profiling.so -- Path to your application"



