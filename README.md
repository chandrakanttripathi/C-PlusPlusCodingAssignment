Objective of this project:
Windows Service and a CLI in C++ that interact using any Inter-Process Communication (IPC) mechanism of your choice. The project focuses on building a robust, interactive system with proper error handling and concurrency.
 
Requirements:
1. ISO C++ 17 Standard
2. Visual Studio 2022 (v143) 

Submission Requirements:
1. Source Code:  
   - Included the complete source code for the Windows Service and CLI.
 
2. Build Instructions:
   - Confirm that Visual Studio 2022 (v143)
   - Set C++ 17 in both NumberCli and NumberService projects.
   - Run build.bat file in powershell or command prompt to create executable.
   
3. Build Script:
   - Run `build.bat` build script to compile and build the code.

4. Running Solution
	- start powershell and run install-service.ps1 script. 
	- Run "sc.exe start NumberService" : This will start the installed service
	- We can run NumberService.exe as console also using command "NumberService.exe --console"
	- Now run NumberCli.exe present in build folder.
	- For unintalling Windows Service, run uninstall-service.ps1
	
5. Data Structure Reasoning:
   - I am using std::map data structure to store number and its timestamp. 
   - insert,delete, find works in O(log n) for map and data is sorted in map
   - If dataset is small or medium, then difference between O(log n) and O(1) is less. We are getting sorted data automatically with this data structure 
   - We can also use hash(unordered_map) in which insert, delete and find runs in  O(1). But for printing in sorted order, we have to call sort function, which takes O(n log n) duration. 
 
6. Notes
   - Used NamedPipe for IPC.
   - map data structure, reason provided in point 5.
   - We have used thread pool to remove thread creation/deletion overhead.
   - Had used proper synchronization for achieving concurrency
   - We can run NumberService as console by running it as "NumberService --console"
   - We can add Logger interface and create different type of concrete Loggers. 

