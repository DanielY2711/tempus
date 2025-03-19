#include <windows.h>
#include <iostream>
#include <string>
#include <psapi.h>


using namespace std;

int main() {
    
    cout << "basic test";

    HWND hwnd = GetForegroundWindow();

    return 0;
}
