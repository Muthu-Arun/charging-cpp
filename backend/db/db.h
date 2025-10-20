#pragma once
#include "sqlite3.h"

namespace Db {
    inline const char *DatabaseFile = "database/user.db";
    struct Sqlite{
    private:
        sqlite3* db;
    public:
        Sqlite(const char* filename);
        ~Sqlite();
        sqlite3* get();
        Sqlite(const Sqlite& other) = delete;
        Sqlite& operator=(const Sqlite& other) = delete;
        Sqlite(Sqlite&& other) noexcept;
        Sqlite& operator=(Sqlite&& other) noexcept;
    };
    struct Stmt{
    private:
        sqlite3_stmt* stmt;
    public:
        Stmt(const char* query, Sqlite& db);
        ~Stmt();
        sqlite3_stmt* get();
        Stmt(const Stmt& other) = delete;
        Stmt& operator=(const Stmt& other) = delete;
        Stmt(Stmt&& other) noexcept;
        Stmt& operator=(Stmt&& other) noexcept;
    };

}