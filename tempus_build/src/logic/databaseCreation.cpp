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
    
    const char* tablenameCheckStatement = "SELECT name FROM sqlite_master WHERE type='table' AND name=?;";
    sqlite3_stmt* sqlStatement = nullptr; 

    void checkSQLError(int ret, sqlite3* db){
        if (ret < 0){
            std::cerr << "sql api error: " << sqlite3_errmsg(db) << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }

    void initialDatabaseSetup(std::string dbname){
        sqlite3* db; 

        int status = sqlite3_open(dbname.c_str(), &db);
        
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

        char* errorMsg = nullptr;

        if (
        sqlite3_exec(db, timelineDbStatement, nullptr, 0, &errorMsg) + 
        sqlite3_exec(db, sumDbStatement, nullptr, 0, &errorMsg) != 0
        ){
            std::cerr << " Error creating tables: " << errorMsg << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }

    void verifyDatabaseIntegrity(std::string path){
        sqlite3* db; 

        if (!std::filesystem::exists(path)){
            initialDatabaseSetup(path); 
        }
        int status = sqlite3_open(path.c_str(), &db);

        if (status < 0){
            std::cerr << "Error opening db"; 
            std::exit(EXIT_FAILURE);
        }

        // verify existance of every table, every column within table
        for (int i = 0; i < tableNames.size(); i++){
            checkSQLError(sqlite3_prepare_v2(db, tablenameCheckStatement, -1, &sqlStatement, nullptr), db);
            checkSQLError(sqlite3_bind_text(sqlStatement, 1, tableNames[i].c_str(), -1, SQLITE_STATIC), db);

            if (sqlite3_step(sqlStatement) != SQLITE_ROW){
                std::cerr << "didnt find table number " << i + 1 << " table name: " << tableNames[i] << std::endl; 
                std::cerr << "sql api error: " << sqlite3_errmsg << std::endl;
                std::exit(EXIT_FAILURE);
            }
            //std::cout << "found table " << reinterpret_cast<const char*>(sqlite3_column_text(sqlStatement,0)) << std::endl; 
            sqlite3_finalize(sqlStatement);

            //Otherwise table is found so check every column,
            std::string tableColCheckStatement = "PRAGMA table_info(" + tableNames[i] + ");";
            

            checkSQLError(sqlite3_prepare_v2(db, tableColCheckStatement.c_str(), -1, &sqlStatement, nullptr), db);
            checkSQLError(sqlite3_bind_text(sqlStatement, 1, tableNames[i].c_str(), -1, SQLITE_STATIC), db);

            int code = sqlite3_step(sqlStatement);
            int col = 0;
            while (col < tableColumns[i].size() && code == SQLITE_ROW){
                std::string curName = reinterpret_cast<const char*>(sqlite3_column_text(sqlStatement,1));
                assert(curName == tableColumns[i][col]);
                //std::cout << "found column: " << curName << std::endl; 
                col++;
                code = sqlite3_step(sqlStatement);
            }
            //std::cout << code << std::endl;

            // Finally assert that the number of col is correct
            if (code != SQLITE_DONE){
                if (code == SQLITE_ROW){
                    std::cout << sqlite3_column_count(sqlStatement) << std::endl;
                    for (int col = 0; col < 6; ++col) {
                        const char* colValue = (const char*)sqlite3_column_text(sqlStatement, col);
                        std::cout << "Column " << col << ": " << (colValue ? colValue : "NULL") << std::endl;
                    } 
                    std::cerr << "unexpected column in table after " << tableColumns[i].size() << std::endl;
                    std::exit(EXIT_FAILURE);
                }
                else{
                    std::cerr << "SQLite error: " << sqlite3_errmsg(db) << std::endl;
                    std::exit(EXIT_FAILURE);
                }
            
            };
            assert(col == tableColumns[i].size());
            sqlite3_finalize(sqlStatement);
            std::cout << "table " << tableNames[i] << " pass" << std::endl; 
        }

    }
    
}
