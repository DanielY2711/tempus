
#include <iostream>
#include <string> 
#include <chrono>
#include <windows.h>
#include "activityTracker.h"

namespace timeline{
    struct timeLineEvent {
        //time related
        std::chrono::sys_days date; 
        std::chrono::system_clock::time_point eventStartTime;
        std::chrono::system_clock::time_point eventEndTime;
        std::chrono::duration<int> duration;
    
        // Window related
        std::wstring processHeader;
        std::wstring processName;
        std::wstring processPath; 

        // Activity Related
        track_activity::activityRating activity;
        };
    
    
    void setWindowInfo(timeLineEvent& currentEvent, HWND& handle);

    std::wstring getCurrentWindowHeader(HWND& handle);

}





