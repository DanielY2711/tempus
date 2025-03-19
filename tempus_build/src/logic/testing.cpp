#include <iostream>
#include "activityTracker.h"
#include <chrono>
#include <cassert>



int main(){
        // Get current time from system_clock
        auto now = std::chrono::system_clock::now();

        // Convert current time to time_t to work with std::tm
        std::time_t current_time = std::chrono::system_clock::to_time_t(now);

        // Convert time_t to tm structure
        std::tm tm = *std::localtime(&current_time);

        // Set time to midnight (00:00:00) today
        tm.tm_hour = 0;
        tm.tm_min = 0;
        tm.tm_sec = 0;

        // Convert tm back to time_t
        std::time_t midnight_time = std::mktime(&tm);

        // Convert back to chrono time_point
        auto midnight = std::chrono::system_clock::from_time_t(midnight_time);

        auto secondsToday = std::chrono::duration_cast<std::chrono::seconds>(now - midnight);

        std::cout << secondsToday.count() << std::endl; 

        auto durationSec = std::chrono::duration_cast<std::chrono::seconds>(midnight.time_since_epoch());

        int seconds = static_cast<int>(durationSec.count());
    
        std::cout << seconds << std::endl; 
        return 0; 

}