#include "db/db.h"
namespace Db {
    Sqlite::Sqlite(const char* filename) {
        if (sqlite3_open(filename, &db) != SQLITE_OK) {
            db = nullptr;
        }
    }
    Sqlite::~Sqlite() {
        if (db) {
            sqlite3_close(db);
        }
    }
    sqlite3* Sqlite::get() {
        return db;
    }
    Sqlite::Sqlite(Sqlite&& other) noexcept : db(other.db) {
        other.db = nullptr;
    }
    Sqlite& Sqlite::operator=(Sqlite&& other) noexcept {
        if (this != &other) {
            if (db) {
                sqlite3_close(db);
            }
            db = other.db;
            other.db = nullptr;
        }
        return *this;
    }
    Stmt::Stmt(const char* query, Sqlite& db) {
        if (sqlite3_prepare_v2(db.get(), query, -1, &stmt, nullptr) != SQLITE_OK) {
            stmt = nullptr;
        }
    }
    Stmt::~Stmt() {
        if (stmt) {
            sqlite3_finalize(stmt);
        }
    }
    sqlite3_stmt* Stmt::get() {
        return stmt;
    }
    Stmt::Stmt(Stmt&& other) noexcept : stmt(other.stmt) {
        other.stmt = nullptr;
    }
    Stmt& Stmt::operator=(Stmt&& other) noexcept {
        if (this != &other) {
            if (stmt) {
                sqlite3_finalize(stmt);
            }
            stmt = other.stmt;
            other.stmt = nullptr;
        }
        return *this;
    }

}