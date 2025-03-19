

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