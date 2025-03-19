#include <windows.h>
#include <iostream>
#include <thread>
#include <semaphore>

std::binary_semaphore activitySemaphore(1);
bool currentSegmentActive = false; 

std::binary_semaphore segmentCountSemaphore(1);
long totalSegments = 0;
long activeSegments = 0; 



struct activityRating{
    long total;
    long active;
};

LRESULT CALLBACK ICUM(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode < 0) {
        return CallNextHookEx(NULL, nCode, wParam, lParam);
    }

    activitySemaphore.acquire(); 
    //std::cout << "I see you moving your mouse" << std::endl; // Flush output
    currentSegmentActive = true; 
    activitySemaphore.release(); 

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT CALLBACK ICUK(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode < 0) {
        return CallNextHookEx(NULL, nCode, wParam, lParam);
    }

    activitySemaphore.acquire(); 
    //std::cout << "I see you typing your keyboard" << std::endl; // Flush output
    currentSegmentActive = true; 
    activitySemaphore.release(); 


    return CallNextHookEx(NULL, nCode, wParam, lParam);
}


void hookAndWait(){
    HHOOK myHook = SetWindowsHookEx(14, ICUM, NULL, 0);
    HHOOK myHook2 = SetWindowsHookEx(13, ICUK, NULL, 0);

    if (!myHook | !myHook2) {
        std::cerr << "Failed to install hook!" << std::endl;
        return;
    }

    std::cout << "Hook installed. Move your mouse!" << std::endl;

    // ✅ Fix: Use a message loop instead of sleeping
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


void segmentActivityTracker(std::chrono::seconds segLength){

    while(true){
        std::this_thread::sleep_for(segLength);

        activitySemaphore.acquire();
        segmentCountSemaphore.acquire();
        if (currentSegmentActive){
            activeSegments++;
            std::cout << "this segment was active \n"; 
        }
        totalSegments++; 
        currentSegmentActive = false; 
        segmentCountSemaphore.release();
        activitySemaphore.release(); 
    }
    

}

activityRating reportActivityAndReset(){


    activityRating output;

    activitySemaphore.acquire();
    segmentCountSemaphore.acquire(); 

    // Prevent segments from being 0 at long segment lengths and low refresh rates. Eg. last segment gets cutoff so lets include anyways
    
    totalSegments++;
    if (currentSegmentActive){
        activeSegments++; 
    }

    output.active = activeSegments; 
    output.total = totalSegments; 


    activeSegments = 0; 
    totalSegments = 0;
    currentSegmentActive = false; 

    segmentCountSemaphore.release();
    activitySemaphore.release(); 

    return output;
}



int main(){
    std::thread hookEventMonitorThread(hookAndWait);
    std::thread segmentActivityThread(segmentActivityTracker, std::chrono::seconds(1));



    while (true){
        std::this_thread::sleep_for(std::chrono::seconds(1));
        activityRating lastActivity = reportActivityAndReset(); 
        std::cout << "Last 10 second segment had " << lastActivity.active << " active segments and " << lastActivity.total << " total segments (5 expected)\n";
    }

    hookEventMonitorThread.join(); 
    segmentActivityThread.join(); 

    return 1;
}