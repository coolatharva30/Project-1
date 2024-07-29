#include <windows.h>
#include <iostream>
#include <tlhelp32.h>
#include <string>
#include <psapi.h>
#include <codecvt>
#include <chrono>
#include <thread>
#include <cstdio>
#include "json.cpp" 
#include <unistd.h>
#include <fstream>
#include <winsock2.h>
#include <ws2tcpip.h>
//#include <google/drive/Drive.h>
using json = nlohmann::json;




std::wstring StringToWString(const std::string& str) {
    return std::wstring(str.begin(), str.end());
}

//Array to store processes
int count = 0;


// Function to get the running time of a process
int GetProcessRunningTime(const std::wstring& pName, DWORD pid) {
    // Open the process with PROCESS_QUERY_INFORMATION access
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
    if (hProcess != NULL) {
        // Get process creation time
        FILETIME createTime, exitTime, kernelTime, userTime;
        if (GetProcessTimes(hProcess, &createTime, &exitTime, &kernelTime, &userTime)) {
            SYSTEMTIME processStartTime;
            FileTimeToSystemTime(&createTime, &processStartTime);

            // Get current time
            SYSTEMTIME currentTime;
            GetSystemTime(&currentTime);

            // Calculate the time difference
            FILETIME ftCurrent, ftStart;
            SystemTimeToFileTime(&currentTime, &ftCurrent);
            SystemTimeToFileTime(&processStartTime, &ftStart);

            ULARGE_INTEGER start, current;
            start.LowPart = ftStart.dwLowDateTime;
            start.HighPart = ftStart.dwHighDateTime;
            current.LowPart = ftCurrent.dwLowDateTime;
            current.HighPart = ftCurrent.dwHighDateTime;

            ULONGLONG duration = current.QuadPart - start.QuadPart;
            duration /= 10000000; // Convert to seconds

            std::wcout << "Process '" << pName << "' with PID " << pid
                       << " has been running for " << duration << " seconds." << std::endl;
            //std::wcout<<duration<<"seconds"<<std::endl;   
		    //return duration;
		} else {
            std::cerr << "Failed to get process times." << std::endl;
        }
        CloseHandle(hProcess);
    } else {
        std::cerr << "Failed to open process." << std::endl;
    }
}

//Function to list processes which are running
void ListProcesses() {
    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;

    // Take a snapshot of all processes in the system
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        std::cerr << "Error: CreateToolhelp32Snapshot failed!" << std::endl;
        return;
    }

    pe32.dwSize = sizeof(PROCESSENTRY32);

    // Retrieve information about the first process and exit if unsuccessful
    if (!Process32First(hProcessSnap, &pe32)) {
        CloseHandle(hProcessSnap);
        std::cerr << "Error: Process32First failed!" << std::endl;
        return;
    }
 
    // Print details of all processes
    std::cout << "PID\tProcess Name" << std::endl;
    
    do {
        //std::cout << pe32.th32ProcessID<<"\t"<<pe32.szExeFile << std::endl;
        
        DWORD pid =  pe32.th32ProcessID;
        std::string pName = pe32.szExeFile; 
        std::wstring wstr = StringToWString(pName);
        
        if(pName=="chrome.exe"){
		std::cout << pid<<"\t"<<pName << std::endl;
		count++;
   
       GetProcessRunningTime( wstr, pid);
       
	}
    } while (Process32Next(hProcessSnap, &pe32));

    // Close the snapshot handle
    CloseHandle(hProcessSnap);
}


//database function





int main(){


 while (true) {
    //no of processes
    
    DWORD processesCount = 0;

    // Take a snapshot of all processes in the system
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        std::cerr << "Error: CreateToolhelp32Snapshot failed!" << std::endl;
        return 1;
    }

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);   //sets the size of PROCESSENTRY32 structure before initialising it.

    // Retrieve information about the first process and exit if unsuccessful
    if (!Process32First(hProcessSnap, &pe32)) {
        CloseHandle(hProcessSnap);
        std::cerr << "Error: Process32First failed!" << std::endl;
        return 1;
    }

    // Count the processes by iterating through the snapshot
    do {
        processesCount++;
    } while (Process32Next(hProcessSnap, &pe32));

    // Close the snapshot handle
    CloseHandle(hProcessSnap);

    std::cout << "Number of processes running: " << processesCount << std::endl;
   
//All the processes 
ListProcesses();
 std::cout<<"number of chrome process:  "<<count<<std::endl;
int process[count];
int pid_no[count];


DWORD pid  =  pe32.th32ProcessID;
        std::string pName = pe32.szExeFile; 
        std::wstring wstr = StringToWString(pName);

 int value = GetProcessRunningTime(wstr,pid);
 pid_no[0] = pid;
 process[0] = value;
std::cout<<"Number of chrome processes"<<count<<" "<<std::endl;

//for(int i=0;i<count;i++){
  //std::cout<<"Pid number is: "<<pid_no[i]<<"\t"<<" and Time of processes is: "<<process[i]<<std::endl;
//}

//Database connection
 json jsonData;
 for (size_t i = 0; i < count; ++i) {
        std::string key = "pid" + std::to_string(i + 1);
        jsonData[key] = pid_no[i];
    }

    for (size_t i = 0; i < count; ++i) {
        std::string key = "time of process" + std::to_string(i + 1);
        jsonData[key] = process[i];
    }


 // Convert JSON object to string
  std::string jsonStr= jsonData.dump();


// Output the JSON string
    std::cout << jsonStr << std::endl;


    // Save the JSON string to a file
    std::ofstream outputFile("output.json"); // Create an output file stream



     if (outputFile.is_open()) {
        outputFile << jsonStr; // Write the JSON string to the file
        outputFile.close(); // Close the file stream
        std::cout << "Output saved to output.json" << std::endl;
    } else {
        std::cerr << "Unable to open file for writing!" << std::endl;
    }
    


    //Transfering of file
     
 // Example include for the Google Drive C++ library

//int main(){
    // Code to authenticate with Google Drive API using OAuth 2.0
    // Initialize Drive API client

    // Read content from the JSON file
    //std::ifstream jsonFile("file.json");
    //std::string content((std::istreambuf_iterator<char>(jsonFile)), std::istreambuf_iterator<char>());

    // Upload the content to Google Drive
    //google::drive::Drive drive; // Assuming a Drive instance
    //google::drive::File file;
    //file.setContent(content);
    //file.setTitle("File Title");
    //file.setDescription("File Description");

    // Upload the file
    //google::drive::File uploadedFile = drive.Files().Insert(file).execute();

    // Check for errors or handle the uploadedFile object
   // std::cout << "File uploaded successfully! ID: " << uploadedFile.getId() << std::endl;

   // return 0;


   sleep(300);
    
   }

}
