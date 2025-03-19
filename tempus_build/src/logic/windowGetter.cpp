#include <windows.h>
#include "windowGetter.h"
#include <string>
#include <locale>
#include <codecvt>
#include <thread>
#include <iostream>
#include <psapi.h>
#include <vector>

#pragma comment(lib, "Psapi.lib")
#pragma comment(lib, "version.lib")



namespace timeline{


    std::wstring getCurrentWindowHeader(HWND& windowHandle){
        wchar_t buffer[256];
        LPWSTR ptr = buffer;
        GetWindowTextW(windowHandle, ptr, sizeof(buffer));
        return std::wstring(buffer);
    }


    void setFriendlyName(std::wstring& processName, WCHAR (&processPath)[MAX_PATH]){

        // Get size of info
        DWORD infoSize = GetFileVersionInfoSizeW(processPath, NULL); 

        // No info or error
        if (infoSize == 0){
            processName = L"";
            return;
        }

        std::vector<BYTE> versionData(infoSize);
        if (!GetFileVersionInfoW(processPath, 0, infoSize, versionData.data())){
            processName = L"";
            return; 
        }

        void* subblockPointer;

        // Try Product Name
        if (VerQueryValueW(versionData.data(), L"\\StringFileInfo\\040904b0\\ProductName", &subblockPointer, NULL)){
            processName = std::wstring(reinterpret_cast<wchar_t*>(subblockPointer));
            return;
        }

        //Try to find name in File Description
        if (VerQueryValueW(versionData.data(), L"\\StringFileInfo\\040904b0\\FileDescription", &subblockPointer, NULL)){
            processName = std::wstring(reinterpret_cast<wchar_t*>(subblockPointer));
            return;
        }
        
        // Couldnt find
        processName = L"";
        return;
    }

    void setWindowInfo(timeLineEvent& currentEvent, HWND& windowHandle){

        currentEvent.processHeader = getCurrentWindowHeader(windowHandle); 

        DWORD processID;
        GetWindowThreadProcessId(windowHandle, &processID);

        HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
        
        // Failed to get handle
        if (processHandle == NULL){
            currentEvent.processName = L"";
            currentEvent.processPath = L""; 
            return;
        }
        
        //Get path
        WCHAR processPath[MAX_PATH];

        //Failed to get path
        if (GetModuleFileNameExW(processHandle, NULL, processPath, MAX_PATH) == 0){
            currentEvent.processName = L"";
            currentEvent.processPath = L""; 
            CloseHandle(processHandle);
            return;
        }
        
        // Set path (for icon fetching later on)
        currentEvent.processPath = processPath;

        setFriendlyName(currentEvent.processName, processPath);

        CloseHandle(processHandle);
    }

}