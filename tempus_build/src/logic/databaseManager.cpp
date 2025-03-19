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
    sqlite3_stmt* sqlStatement;

    // Path to the database
    std::string dbPath = "events.db";

    // Database object pointer
    sqlite3 *eventsDB; 

    int timepointToInt(std::chrono::system_clock::time_point point){
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(point.time_since_epoch());
        return static_cast<int>(duration.count());
    }

    void checkSQLError(int ret){
        if (ret < 0){
            std::cerr << "sql api error: " << sqlite3_errmsg << std::endl;
            std::exit(EXIT_FAILURE);
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

    void checkDateChange(){


        checkSQLError(sqlite3_prepare_v2(eventsDB, tuplesBeforeToday, -1, &sqlStatement, nullptr)); 
        checkSQLError(sqlite3_bind_int(sqlStatement, 1, getMidnight(std::chrono::system_clock::now())));

        // Found entries from before today, flush is needed
        if (sqlite3_step(sqlStatement) == SQLITE_ROW){
            flushDailyDB(); 
        }

    }
    //Function that should run when the app opens
    void startupDBMngr(){
        verifyDatabaseIntegrity(dbPath);
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


        // Now for sum table, we must ensure that each header only gets 1 tuple per day
        checkSQLError(sqlite3_prepare_v2(eventsDB, getExistingTuple, -1, &sqlStatement, nullptr));
        checkSQLError(sqlite3_bind_text16(sqlStatement, 1, event.processHeader.c_str(), -1, SQLITE_STATIC));
        checkSQLError(sqlite3_bind_int(sqlStatement, 2, date));
        
        // First time seeing this header today we just insert
        if (sqlite3_step(sqlStatement) != SQLITE_ROW){
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

            checkSQLError(sqlite3_prepare_v2(eventsDB, updateTuple, -1, &sqlStatement, nullptr)); 
            checkSQLError(sqlite3_bind_int(sqlStatement, 1, newDuration));
            checkSQLError(sqlite3_bind_int(sqlStatement, 2, newActiveIntervals));
            checkSQLError(sqlite3_bind_int(sqlStatement, 3, newTotalIntervals));
            checkSQLError(sqlite3_bind_text16(sqlStatement, 4, event.processHeader.c_str(), -1, SQLITE_STATIC));
            checkSQLError(sqlite3_bind_int(sqlStatement, 5, date)); 
            
            checkSQLError(sqlite3_step(sqlStatement));
        }
    }


    // Function that updates entries in the daily database
    void updateDailyDB(std::vector<timeline::timeLineEvent> newEvents){
        for (int ind = 0; ind < newEvents.size(); ind++){
            processEvent(newEvents[ind]);
        }
    }


    // Function that flushes entries in the daily database to the all time database
    // Not needed in current setup
    void flushDailyDB(){
        //
        sqlite3_stmt* inserStatement;

        checkSQLError(sqlite3_prepare_v2(eventsDB, getAllTuples, -1, &sqlStatement, nullptr));
        
        while (sqlite3_step(sqlStatement) == SQLITE_ROW){

        }
        
    }
}


int main() {


    return 0;
}
