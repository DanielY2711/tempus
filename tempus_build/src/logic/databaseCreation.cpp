#include <iostream>
#include <sqlite3.h>
#include <filesystem>
#include <vector>
#include <cassert>

namespace dbmng{

    // const std::vector<std::string> tableNames = {"daily", "dailySum", "allTime", "allTimeSum"};
    // const std::vector<std::vector<std::string>> tableColumns = {{"id", "header", "program", "startTime", "duration", "activeIntervals", "totalIntervals"},
    //                                                             {"id", "header", "program", "duration", "activeIntervals", "totalIntervals"},
    //                                                             {"id", "header", "program", "startTime", "duration", "activeIntervals", "totalIntervals"},
    //                                                             {"id", "header", "program", "date", "duration", "activeIntervals", "totalIntervals"}};

    const std::vector<std::string> tableNames = {"timeline", "allTimeSum"};
    const std::vector<std::vector<std::string>> tableColumns = {{"id", "header", "program", "startTime", "duration", "activeIntervals", "totalIntervals"},
                                                                {"id", "header", "program", "date", "duration", "activeIntervals", "totalIntervals"}};    
    
    const char* tablenameCheckStatement = "SELECT name FROM sqlite_master WHERE type='table' AND name=?";
    const char* tableColCheckStatement = "PRAGMA table_info(?)";
    sqlite3_stmt* sqlStatement; 


    void checkSQLError(int ret){
        if (ret < 0){
            std::cerr << "sql api error: " << sqlite3_errmsg << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }

    void verifyDatabaseIntegrity(std::string path){

        if (!std::filesystem::exists(path)){
            initialDatabaseSetup(); 
        }
        sqlite3 *db;

        checkSQLError(sqlite3_open(path.c_str(), &db));

        // verify existance of every table, every column within table
        for (int i = 0; i < tableNames.size(); i++){
            checkSQLError(sqlite3_prepare_v2(db, tablenameCheckStatement, -1, &sqlStatement, nullptr));
            checkSQLError(sqlite3_bind_text(sqlStatement, 1, tableNames[i].c_str(), -1, SQLITE_STATIC));

            if (sqlite3_step(sqlStatement) != SQLITE_ROW){
                std::cerr << "didnt find table number " << i + 1 << std::endl; 
                std::exit(EXIT_FAILURE);
            }
            
            //Otherwise table is found so check every column
        
            checkSQLError(sqlite3_prepare_v2(db, tableColCheckStatement, -1, &sqlStatement, nullptr));
            checkSQLError(sqlite3_bind_text(sqlStatement, 1, tableNames[i].c_str(), -1, SQLITE_STATIC));
            
            int col = 0;
            while (col < tableColumns[i].size() && sqlite3_step(sqlStatement) == SQLITE_ROW){
                std::string curName = reinterpret_cast<const char*>(sqlite3_column_text(sqlStatement,1));
                assert(curName == tableColumns[i][col]);
                col += 1;
            }
            // Finally assert that the number of col is correct
            assert(sqlite3_step(sqlStatement) == SQLITE_DONE && col == tableColumns[i].size());
        }

    }



    void initialDatabaseSetup(){
        sqlite3 *eventsDB;
        int status = sqlite3_open("events.db", &eventsDB);
        
        if (status){
            std::cerr << "Error initializing db " << status << std::endl;
            std::exit(EXIT_FAILURE);
        }

        const char* timelineDbStatement = R"(
            CREATE TABLE IF NOT EXISTS timeline (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            header TEXT NOT NULL,
            program TEXT, 
            startTime INTEGER NOT NULL,
            duration INTEGER NOT NULL,
            activeIntervals INTEGER,
            totalIntervals INTEGER NOT NULL
            );
        )";
        
        const char* sumDbStatement = R"(
            CREATE TABLE IF NOT EXISTS allTimeSum (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            header TEXT NOT NULL,
            program TEXT, 
            date INTEGER NOT NULL,
            duration INTEGER NOT NULL,
            activeIntervals INTEGER,
            totalIntervals INTEGER NOT NULL
            );
        )";

        // const char* dailyDbStatement = R"(
        //     CREATE TABLE IF NOT EXISTS daily (
        //     id INTEGER PRIMARY KEY AUTOINCREMENT,
        //     header TEXT NOT NULL,
        //     program TEXT, 
        //     startTime INTEGER NOT NULL,
        //     duration INTEGER NOT NULL,
        //     activeIntervals INTEGER,
        //     totalIntervals INTEGER NOT NULL
        //     );
        // )";

        // const char* dailySumDbStatement = R"(
        //     CREATE TABLE IF NOT EXISTS dailySum (
        //     id INTEGER PRIMARY KEY AUTOINCREMENT,
        //     header TEXT UNIQUE NOT NULL,
        //     program TEXT, 
        //     duration INTEGER NOT NULL,
        //     activeIntervals INTEGER,
        //     totalIntervals INTEGER NOT NULL
        //     );
        // )";

        // // not used currently
        // const char* timelineDbStatement = R"(
        //     CREATE TABLE IF NOT EXISTS timeline (
        //     id INTEGER PRIMARY KEY AUTOINCREMENT,
        //     header TEXT NOT NULL,
        //     program TEXT, 
        //     startTime INTEGER NOT NULL,
        //     duration INTEGER NOT NULL,
        //     activeIntervals INTEGER,
        //     totalIntervals INTEGER NOT NULL
        //     );
        // )";
        
        char* errorMsg = nullptr;

        // if (sqlite3_exec(eventsDB, dailyDbStatement, nullptr, 0, &errorMsg) + 
        //     sqlite3_exec(eventsDB, mainDbStatement, nullptr, 0, &errorMsg) + 
        //     sqlite3_exec(eventsDB, dailySumDbStatement, nullptr, 0, &errorMsg) +
        //     sqlite3_exec(eventsDB, mainSumDbStatement, nullptr, 0, &errorMsg)  != 0){
        //         std::cerr << " Error creating tables: " << errorMsg << std::endl;
        //         std::exit(EXIT_FAILURE);
        // }

        if (
        sqlite3_exec(eventsDB, timelineDbStatement, nullptr, 0, &errorMsg) + 
        sqlite3_exec(eventsDB, sumDbStatement, nullptr, 0, &errorMsg) != 0
        ){
            std::cerr << " Error creating tables: " << errorMsg << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }
    
}


// int main(){
//     dbmng::initialDatabaseSetup(); 
//     const char* insertStatement = R"(
//         INSERT INTO daily (header, program, startTime, duration, activeIntervals, totalIntervals)
//         VALUES ('how not to shart', 'google chrome', 1000, 2000, 3, 6);
//         )"; 
    
//     sqlite3 *eventsDB;
//     int status = sqlite3_open("events.db", &eventsDB);

//     if (status){
//         std::cerr << "Database was not created\n";
//         return -1;
//     }
//     char * insertMsg = nullptr;

//     if (sqlite3_exec(eventsDB, insertStatement, nullptr, 0, &insertMsg)){
//         std::cerr << "failed to insert " << insertMsg << std::endl;
//         sqlite3_close(eventsDB);
//         return -1;
//     }

//     const char* selectQuery = R"(
//         SELECT * FROM daily;
//         )";

//     sqlite3_stmt* selectStmt; 

//     if (sqlite3_prepare_v2(eventsDB, selectQuery, -1, &selectStmt, nullptr)){
//         std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(eventsDB) << std::endl;
//         sqlite3_close(eventsDB);
//         return -1;
//     }

//     // Step through the results
//     while (sqlite3_step(selectStmt) == SQLITE_ROW) {
//         int column_count = sqlite3_column_count(selectStmt);
//         for (int i = 0; i < column_count; ++i) {
//             const char* value = reinterpret_cast<const char*>(sqlite3_column_text(selectStmt, i));
//             std::cout << (value ? value : "NULL") << " ";
//         }
//         std::cout << std::endl;
//     }

//     sqlite3_finalize(selectStmt);
//     sqlite3_close(eventsDB);

//     return 0; 

// }