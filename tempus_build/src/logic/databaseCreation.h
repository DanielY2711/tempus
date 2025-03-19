#include <sqlite3.h>

namespace dbmng{
    void initialDatabaseSetup();
    void verifyDatabaseIntegrity(std::string path);
    extern sqlite3_stmt* sqlStatement; 
}