#include <YSI_Coding\y_hooks>

#define DATABASE_LOCATION "LS80/server.db"
#define SERVER_TAG "LS80"


new DB:server_database;


stock OpenDatabase() {
    server_database = db_open(DATABASE_LOCATION);
    if (server_database == DB:0) {
        new error_msg[128];
        format(error_msg, sizeof(error_msg), "[%s] Failed to connect to SQLite database: scriptfiles/%s", SERVER_TAG, DATABASE_LOCATION);
        printf(error_msg);
        SendRconCommand("exit");
        return 0;
    }
    new success_msg[128];
    format(success_msg, sizeof(success_msg), "[%s] Successfully connected to SQLite database: scriptfiles/%s", SERVER_TAG, DATABASE_LOCATION);
    printf(success_msg);
    CreateTables();
    return 1;
}


stock CloseDatabase() {
    if (server_database != DB:0) {
        new close_msg[128];
        format(close_msg, sizeof(close_msg), "[%s] SQLite database connection closed.", SERVER_TAG);
        printf(close_msg);
        db_close(server_database);
        server_database = DB:0;
    }
    return 1;
}


stock CreateTables() {
    new query[512];
    // Users table
    format(query, sizeof(query), "CREATE TABLE IF NOT EXISTS users (\
        userId INTEGER PRIMARY KEY AUTOINCREMENT, \
        username TEXT UNIQUE, \
        password TEXT)");
    db_query(server_database, query);
    // Characters table
    format(query, sizeof(query), "CREATE TABLE IF NOT EXISTS characters (\
        charId INTEGER PRIMARY KEY AUTOINCREMENT, \
        userId INTEGER, \
        name TEXT, \
        slot INTEGER, \
        race TEXT, \
        height FLOAT, \
        gender TEXT, \
        FOREIGN KEY(userId) REFERENCES users(userId))");
    db_query(server_database, query);
    new table_msg[128];
    format(table_msg, sizeof(table_msg), "[%s] Created or verified 'users' and 'characters' tables.", SERVER_TAG);
    printf(table_msg);
}