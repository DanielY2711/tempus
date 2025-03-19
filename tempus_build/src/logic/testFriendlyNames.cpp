#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <psapi.h>

#pragma comment(lib, "Psapi.lib")
#pragma comment(lib, "version.lib")

std::wstring GetProcessFriendlyName() {

    wchar_t text[256];
    int maxChar = sizeof(text);
    wchar_t * ptr = text; 

    HWND cur = GetForegroundWindow();
    GetWindowTextW(cur, ptr, maxChar);

    DWORD pid;
    DWORD tid = GetWindowThreadProcessId(cur, &pid);


    // Open the process with read access
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (hProcess == NULL) {
        std::cerr << "Error opening process" << std::endl;
        return L"";
    }

    // Get the full path to the executable
    WCHAR exePath[MAX_PATH];
    if (GetModuleFileNameExW(hProcess, NULL, exePath, MAX_PATH) == 0) {
        std::cerr << "Error getting executable path" << std::endl;
        CloseHandle(hProcess);
        return L"";
    }

    // Get version information from the executable
    DWORD dummy;
    DWORD dwSize = GetFileVersionInfoSizeW(exePath, &dummy);
    if (dwSize == 0) {
        std::cerr << "Error getting version info size" << std::endl;
        CloseHandle(hProcess);
        return L"";
    }

    std::vector<BYTE> versionData(dwSize);
    if (!GetFileVersionInfoW(exePath, 0, dwSize, versionData.data())) {
        std::cerr << "Error getting version info" << std::endl;
        CloseHandle(hProcess);
        return L"";
    }

    // Query the file description or product name
    void* lpBuffer;
    UINT uLen;
    if (VerQueryValueW(versionData.data(), L"\\StringFileInfo\\040904b0\\FileDescription", &lpBuffer, &uLen)) {
        // Return the friendly name (file description)
        CloseHandle(hProcess);
        return std::wstring(reinterpret_cast<wchar_t*>(lpBuffer));
    }

    // If FileDescription is not found, try ProductName
    if (VerQueryValueW(versionData.data(), L"\\StringFileInfo\\040904b0\\ProductName", &lpBuffer, &uLen)) {
        CloseHandle(hProcess);
        return std::wstring(reinterpret_cast<wchar_t*>(lpBuffer));
    }

    // If no friendly name found, return empty string
    CloseHandle(hProcess);
    return L"";
}

int main() {
    DWORD pid = 1234;  // Replace with the PID you want to check
    std::wstring friendlyName = GetProcessFriendlyName();
    if (!friendlyName.empty()) {
        std::wcout << L"Friendly name: " << friendlyName << std::endl;
    } else {
        std::wcout << L"Could not retrieve friendly name" << std::endl;
    }
    return 0;
}
