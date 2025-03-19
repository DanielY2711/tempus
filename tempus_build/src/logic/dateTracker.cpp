
#include <chrono>
#include <ctime>
#include <iostream>
#include <thread>


namespace trackdate{
    
    namespace chr = std::chrono;

    void updateAtMidnight(chr::sys_days& dateVar){
        
        while (true){
            std::time_t now_time_t = chr::system_clock::to_time_t(chr::system_clock::now());

            std::tm now_tm = *std::localtime(&now_time_t);
    
            std::tm tmrw_tm = now_tm; 
    
            tmrw_tm.tm_hour = 0;
            tmrw_tm.tm_min = 0;
            tmrw_tm.tm_sec = 0;
            tmrw_tm.tm_mday += 1; 
            
            std::time_t tmrw_t = std::mktime(&tmrw_tm);
    
            auto tmrw = std::chrono::system_clock::from_time_t(tmrw_t);
    
            auto dur = tmrw - chr::system_clock::now(); 
            long idur = std::chrono::duration_cast<std::chrono::seconds>(dur).count();
            std::cout << "sleeping for " << idur << std::endl;
            
            std::this_thread::sleep_for(dur);
    
            std::cout << "thread wakes to change value\n";
    
            dateVar += chr::days(1);
        }


    }

}


