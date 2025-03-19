
#include "activityTracker.h"
#include <thread>
#include <chrono>
#include <windows.h> 
#include <semaphore>
#include <iostream>
#include <atomic>


namespace {



    std::binary_semaphore activitySemaphore(1);
    bool currentSegmentActive = false; 

    std::binary_semaphore segmentCountSemaphore(1);
    long totalSegments = 0;
    long activeSegments = 0; 

    // Callback function
    LRESULT CALLBACK trackUserInput(int nCode, WPARAM wParam, LPARAM lParam) {
        if (nCode < 0) {
            return CallNextHookEx(NULL, nCode, wParam, lParam);
        }

        activitySemaphore.acquire(); 
        currentSegmentActive = true; 
        activitySemaphore.release(); 
        return CallNextHookEx(NULL, nCode, wParam, lParam);
    }


    void setUpHooks(){
        HHOOK myHook = SetWindowsHookEx(14, trackUserInput, NULL, 0);
        HHOOK myHook2 = SetWindowsHookEx(13, trackUserInput, NULL, 0);
    
        if (!myHook | !myHook2) {
            std::cerr << "Failed to install hook!" << std::endl;
            return;
        }
    
        std::cout << "Hook installed. Move your mouse!" << std::endl;
    
        // force blocked state on thread
        MSG msg;
        while (GetMessage(&msg, NULL, 0, 0)) {
            //TranslateMessage(&msg);
            //DispatchMessage(&msg);
            //std::cout << "Message recieved" << std::endl;
        }
    
        UnhookWindowsHookEx(myHook);
        std::cout << "Hook and wait exit \n";
        return;
    }
    
    /*
        Runs in a separate thread. Sleeps for timeoutInterval, then wakes and checks if the hooks detected any movement from the user. 
        If movement is detected, the time interval is considered active, otherwise the time interval is considered inactive.
        The %idle can be calculated as (total_intervals - active_intervals) / (total_intervals) * 100 
    */
    void checkActivity(std::chrono::seconds timeoutInterval){
        
        while (true){
            std::this_thread::sleep_for(timeoutInterval);

            activitySemaphore.acquire();
            segmentCountSemaphore.acquire();
            if (currentSegmentActive){
                activeSegments++;
                //std::cout << "this segment was active \n"; 
            }
            totalSegments++; 
            currentSegmentActive = false; 
            segmentCountSemaphore.release();
            activitySemaphore.release(); 
        }
    }
}

namespace track_activity{

    activityRating reportActivityThenReset(){
        
        activityRating output;

        activitySemaphore.acquire();
        segmentCountSemaphore.acquire(); 
    
        // Prevent segments from being 0 at long segment lengths and low refresh rates. Eg. last segment gets cutoff so lets include anyways

        totalSegments++;
        if (currentSegmentActive){
            activeSegments++; 
        }
    
        // Semaphore aquisition prevents new activity tracking until the current activity is processed and passed on
        output.active = activeSegments; 
        output.total = totalSegments; 
    
        activeSegments = 0; 
        totalSegments = 0;
        currentSegmentActive = false; 
    
        segmentCountSemaphore.release();
        activitySemaphore.release(); 
    
        return output;
    }

    void startActivityTracking(std::chrono::seconds timeoutInterval){
        std::thread hookEventMonitorThread(setUpHooks);
        std::thread segmentActivityThread(checkActivity, std::chrono::seconds(timeoutInterval));
    
        // detach both so when this thread ends they can continue
        hookEventMonitorThread.detach(); 
        segmentActivityThread.detach();

    }

}