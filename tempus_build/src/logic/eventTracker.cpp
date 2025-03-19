#include <vector>
#include "windowGetter.h"
#include <thread>
#include <ctime>
#include <iostream>
#include <chrono>
#include <windows.h>
#include <mutex>



namespace timeline {

    namespace chr = std::chrono;
    
    class timelineObject{
        private:
            timeLineEvent lastEvent;
            chr::sys_days currentDate;
            std::mutex timelineMutex;

            void cutCurrentEvent(HWND& currentEventHandle){
                lastEvent.eventEndTime = chr::system_clock::now();
                lastEvent.duration = std::chrono::duration_cast<std::chrono::seconds>(lastEvent.eventEndTime - lastEvent.eventStartTime);
                lastEvent.activity = track_activity::reportActivityThenReset(); 

                timelineMutex.lock(); 
                timeline.push_back(lastEvent);
                timelineMutex.unlock(); 

                // Update the current event
                setWindowInfo(lastEvent, currentEventHandle);
                lastEvent.eventStartTime = chr::system_clock::now();
            }
            
        public:
            std::vector<timeLineEvent> timeline;

            // Checking for changes in the focused window
            void updateTimeline(){
                
                HWND currentEventHandle = GetForegroundWindow(); 
                std::wstring currentHeader = getCurrentWindowHeader(currentEventHandle); 


                // Add event to timeline
                if (lastEvent.processHeader != currentHeader){
                    //std::cout << "registering new event \n";
                    cutCurrentEvent(currentEventHandle); 
                }
            }

            // Initializer
            timelineObject() 
                : timeline(),
                lastEvent()
            {
                HWND currentEventHandle = GetForegroundWindow(); 
                lastEvent.eventStartTime = chr::system_clock::now();
                track_activity::startActivityTracking(std::chrono::minutes(1));
                setWindowInfo(lastEvent, currentEventHandle); 

            }

            std::vector<timeLineEvent> dumpCachedTimeline(){
                
                //TO-DO: semaphore for safety when working with timeline vector
                
                
                // Cut off current event
                HWND currentEventHandle = GetForegroundWindow(); 
                cutCurrentEvent(currentEventHandle);
                
                timelineMutex.lock(); 
                std::vector<timeLineEvent> vectorForProcessing = timeline;
                timeline.clear();
                timelineMutex.unlock();
                
                // TO-DO - function that processes these timeline events
            }
             
    };


    void createTimelineLoop(std::chrono::seconds refreshRate){
        timelineObject tObj;

        int prev = 0;
        while (true){
            
            while (tObj.timeline.size() < prev+5){
                
                tObj.updateTimeline();
                std::this_thread::sleep_for(chr::milliseconds(500));
            }

            // Then print out the vector
            for (timeLineEvent event : tObj.timeline){
                std::time_t startTimeC = std::chrono::system_clock::to_time_t(event.eventStartTime);
                int count = std::chrono::duration_cast<std::chrono::seconds>(event.duration).count();
                std::wcout << event.processHeader << " from " << std::ctime(&startTimeC) << " for " << count << "\n"; 
            }

            prev = tObj.timeline.size();

        }
    }
};


int main(){
    timeline::createTimelineLoop(std::chrono::seconds(15));
    //windowTracker wt;
    //std::cout << wt.getWindowHeader();
    return 1;
}
