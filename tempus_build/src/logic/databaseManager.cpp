#include <iostream>
#include <sqlite3.h>
#include <filesystem>
#include "databaseCreation.h"
#include "windowGetter.h"
#include <vector>

namespace dbmng{

    //Get existing sum tuple statment
    const char* getExistingTuple = "SELECT duration, activeIntervals, totalIntervals FROM allTimeSum WHERE header = ? AND date = ?;";

    //Insert tuple statement
    const char* insertNewTuple = 
        R"(INSERT INTO timeline (header, program, startTime, duration, activeIntervals, totalIntervals)
        VALUE(?, ?, ?, ?, ?, ?);
    )";

    const char* insertNewSumTuple =
        R"(INSERT INTO allTimeSum (header, program, date, duration, activeIntervals, totalIntervals)
        VALUE(?, ?, ?, ?, ?, ?);
    )";
    
    //unused
    const char* getAllTuples = 
        R"(SELECT header, program, startTime, duration, activeIntervals, totalIntervals
           FROM daily;)"; 

    // unused
    const char* insertAllTimeTuple = 
        R"(INSERT INTO allTime (header, program, startTime, duration, activeIntervals, totalIntervals)
        VALUE(?,?,?,?,?,?)
        )"; 
    
    //Update tuple statement
    const char* updateTuple = 
        R"(UPDATE allTimeSum
            SET duration = ?, activeIntervals = ?, totalIntervals = ?
            WHERE header = ? AND date = ?;
        )";

    // select tuples in the daily table from before a certain time
    const char* tuplesBeforeToday = 
        R"(SELECT id FROM daily WHERE startTime < ?;)"; 

    
    //Where statements return
    //sqlite3_stmt* sqlStatement;

    // Path to the database
    std::string dbPath = "test.db";

    // Database object pointer
    sqlite3 *eventsDB; 

    int timepointToInt(std::chrono::system_clock::time_point point){
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(point.time_since_epoch());
        return static_cast<int>(duration.count());
    }

    void checkSQLError(int ret){
        if (ret != 0){
            std::cerr << "sql api error: " << sqlite3_errmsg(eventsDB) << std::endl;
            //std::exit(EXIT_FAILURE);
        }
    }


    int getMidnight(std::chrono::system_clock::time_point point){
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

        return timepointToInt(midnight); 
    }


    //Function that should run when the app opens
    void startupDBMngr(){
        verifyDatabaseIntegrity(dbPath);

        int status = sqlite3_open(dbPath.c_str(), &eventsDB);

        if (status < 0){
            std::cerr << "Error opening db"; 
            std::exit(EXIT_FAILURE);
        }

    }

    // Function for adding new events into the daily and dailySum tables
    void processEvent(timeline::timeLineEvent event){
        int date = getMidnight(event.eventStartTime);
        // First we guranteed need to add the event into the timeline table
        checkSQLError(sqlite3_prepare_v2(eventsDB, insertNewTuple, -1, &sqlStatement, nullptr));

        checkSQLError(sqlite3_bind_text16(sqlStatement, 1, event.processHeader.c_str(), -1, SQLITE_STATIC));
        checkSQLError(sqlite3_bind_text16(sqlStatement, 2, event.processName.c_str(), -1, SQLITE_STATIC));
        checkSQLError(sqlite3_bind_int(sqlStatement, 3, timepointToInt(event.eventStartTime)));
        checkSQLError(sqlite3_bind_int(sqlStatement, 4, event.duration.count()));
        checkSQLError(sqlite3_bind_int(sqlStatement, 5, event.activity.active));
        checkSQLError(sqlite3_bind_int(sqlStatement, 6, event.activity.total)); 

        checkSQLError(sqlite3_step(sqlStatement));

        checkSQLError(sqlite3_finalize(sqlStatement)); 

        // Now for sum table, we must ensure that each header only gets 1 tuple per day
        checkSQLError(sqlite3_prepare_v2(eventsDB, getExistingTuple, -1, &sqlStatement, nullptr));
        checkSQLError(sqlite3_bind_text16(sqlStatement, 1, event.processHeader.c_str(), -1, SQLITE_STATIC));
        checkSQLError(sqlite3_bind_int(sqlStatement, 2, date));
        
        // First time seeing this header today we just insert
        if (sqlite3_step(sqlStatement) != SQLITE_ROW){
            
            checkSQLError(sqlite3_finalize(sqlStatement)); 

            checkSQLError(sqlite3_prepare16_v2(eventsDB, insertNewSumTuple, -1, &sqlStatement, nullptr));
            checkSQLError(sqlite3_bind_text16(sqlStatement, 1, event.processHeader.c_str(), -1, SQLITE_STATIC));
            checkSQLError(sqlite3_bind_text16(sqlStatement, 2, event.processName.c_str(), -1, SQLITE_STATIC));
            checkSQLError(sqlite3_bind_int(sqlStatement, 3, date));
            checkSQLError(sqlite3_bind_int(sqlStatement, 3, event.duration.count()));
            checkSQLError(sqlite3_bind_int(sqlStatement, 4, event.activity.active));
            checkSQLError(sqlite3_bind_int(sqlStatement, 5, event.activity.total)); 
    
            checkSQLError(sqlite3_step(sqlStatement));
        }
        // Seen header before, must update values
        else{
            // Get previous values
            int newDuration = sqlite3_column_int(sqlStatement, 0) + event.duration.count();
            int newActiveIntervals = sqlite3_column_int(sqlStatement, 1) + event.activity.active;
            int newTotalIntervals = sqlite3_column_int(sqlStatement, 2) + event.activity.total;

            checkSQLError(sqlite3_finalize(sqlStatement)); 

            checkSQLError(sqlite3_prepare_v2(eventsDB, updateTuple, -1, &sqlStatement, nullptr)); 
            checkSQLError(sqlite3_bind_int(sqlStatement, 1, newDuration));
            checkSQLError(sqlite3_bind_int(sqlStatement, 2, newActiveIntervals));
            checkSQLError(sqlite3_bind_int(sqlStatement, 3, newTotalIntervals));
            checkSQLError(sqlite3_bind_text16(sqlStatement, 4, event.processHeader.c_str(), -1, SQLITE_STATIC));
            checkSQLError(sqlite3_bind_int(sqlStatement, 5, date)); 
            
            checkSQLError(sqlite3_step(sqlStatement));
        }
        checkSQLError(sqlite3_finalize(sqlStatement)); 
    }


    // Function that adds newly tracked events to the databases
    void updateDB(std::vector<timeline::timeLineEvent> newEvents){
        for (int ind = 0; ind < newEvents.size(); ind++){
            processEvent(newEvents[ind]);
        }
    }


    // Function that flushes entries in the daily database to the all time database
    // Not needed in current setup
    void flushDailyDB(){
        sqlite3_stmt* inserStatement;

        checkSQLError(sqlite3_prepare_v2(eventsDB, getAllTuples, -1, &sqlStatement, nullptr));
        
        while (sqlite3_step(sqlStatement) == SQLITE_ROW){

        }
        
    }
}


int main() {
    

    timeline::timeLineEvent event1;
    timeline::timeLineEvent event2;
    timeline::timeLineEvent event3;
    timeline::timeLineEvent event4;

    std::chrono::days day1(1);  // 1-day duration
    std::chrono::hours hour1(1);
    std::chrono::time_point now = std::chrono::system_clock::now(); 
    
    event1.eventStartTime = now; 
    event1.duration = hour1; 
    event1.processHeader = L"New Tab";
    event1.processName = L"Chrome";
    event1.activity.active = 10;
    event1.activity.total = 100; 

    event2.eventStartTime = now + hour1; 
    event2.duration = hour1; 
    event2.processHeader = L"New Tab";
    event2.processName = L"Chrome";
    event2.activity.active = 10;
    event2.activity.total = 100; 

    event3.eventStartTime = now - day1; 
    event3.duration = hour1 + hour1; 
    event3.processHeader = L"New Tab";
    event3.processName = L"Chrome";
    event3.activity.active = 10;
    event3.activity.total = 100; 

    event4.eventStartTime = now + hour1 + hour1; 
    event4.duration = hour1 + hour1; 
    event4.processHeader = L"New Tab 2";
    event4.processName = L"Chrome 2";
    event4.activity.active = 10;
    event4.activity.total = 100; 

    std::vector<timeline::timeLineEvent> events = {event1, event2, event3, event4}; 

    dbmng::updateDB(events);

    const char* timelineS = "SELECT * FROM timeline;";
    const char* allTimeSum = "SELECT * FROM allTimeSum";

    sqlite3_prepare_v2(dbmng::eventsDB, timelineS, -1, &dbmng::sqlStatement, nullptr);

    while (sqlite3_step(dbmng::sqlStatement) == SQLITE_ROW){
        int ncol = sqlite3_column_count(dbmng::sqlStatement);
        for (int col = 0; col < ncol; ++col) {
            const char* colValue = (const char*)sqlite3_column_text(dbmng::sqlStatement, col);
            std::cout << "Column " << col << ": " << (colValue ? colValue : "NULL") << " ";
        } 
        std::cout << std::endl;
    }

    return 0;
}
