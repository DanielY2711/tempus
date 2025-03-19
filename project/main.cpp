#include <iostream>
#include <windows.h>
#include <conio.h>
#include <thread>

int main() {





    wchar_t text[256];
    int maxChar = sizeof(text);
    wchar_t * ptr = text; 
    HWND prev = 0; 
    bool flag = true;

    while (flag){
        if (_kbhit()){
            char ch = _getch();

            if (ch == ' '){
                std::cout << "Shutting down \n";
                flag = false;
            }
        }

        else{
            HWND cur = GetForegroundWindow();

            if (cur != prev){
                GetWindowTextW(cur, ptr, maxChar);
                std::cout << "currently focused on: ";
                std::wcout << text << "\n";
                prev = cur;
            }
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
 
    return 0;
}
