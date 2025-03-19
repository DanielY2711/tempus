#include <string>
#include <chrono>
#include <ctime>
#include <iostream>


std::string getTime(){
    auto now = std::chrono::system_clock::now();
    
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::cout << now_c << "\n";
    struct tm tm; // time struct
    localtime_s(&tm, &now_c);

    char buffer[20];

    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm);

    return std::string(buffer);
}


