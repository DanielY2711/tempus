

-- Creating tables

CREATE TABLE IF NOT EXISTS daily (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    header TEXT NOT NULL,
    program TEXT, 
    startTime INTEGER NOT NULL,
    duration INTEGER NOT NULL,
    activeIntervals INTEGER,
    totalIntervals INTEGER NOT NULL,
);

CREATE TABLE IF NOT EXISTS allTime (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    header TEXT NOT NULL,
    program TEXT, 
    startTime INTEGER NOT NULL,
    duration INTEGER NOT NULL,
    activeIntervals INTEGER,
    totalIntervals INTEGER NOT NULL,
);


INSERT INTO daily (header, program, startTime, duration, activeIntervals, totalIntervals)
VALUE ('how not to shart', 'google chrome', 1000, 2000, 3, 6)


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