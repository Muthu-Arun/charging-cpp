#include "rest/charging.h"
#include "crow/http_request.h"
#include "crow/http_response.h"
#include "db/db.h"
#include <string>

namespace Charging {
crow::response initiate_charge(const crow::request &req) {
    crow::json::rvalue body = crow::json::load(req.body);
    if (!body || !body.has("outlet_id")) {
        return crow::response(400, "Invalid request body");
    }
    const int user_id = Validate::get_userid_from_request(req);
    if (user_id == -1) {
        return crow::response(401, "Unauthorized");
    }
    const int outlet_id = body["outlet_id"].i();
    Db::Sqlite db(Db::DatabaseFile);
    // Checking the availability of the outlet and occupying it must be in a
    // single transaction
    sqlite3_exec(db.get(), "begin transaction", nullptr, nullptr, nullptr);
    {
        constexpr const char *query = "SELECT STATUS FROM OUTLET WHERE ID = ?";
        Db::Stmt stmt(query, db);
        sqlite3_bind_text(stmt.get(), 1, query, -1, nullptr);
        if (sqlite3_step(stmt.get()) != SQLITE_DONE) {
            sqlite3_exec(db.get(), "end transaction", nullptr, nullptr,
                         nullptr);
            return crow::response(
                500,
                "Internal Server Error -> Unable to Check Outlet Availability");
        }
        const unsigned char *availability = sqlite3_column_text(stmt.get(), 1);
        if (std::string(reinterpret_cast<const char *>(availability)) !=
            "AVAILABLE") {
            sqlite3_exec(db.get(), "end transaction", nullptr, nullptr,
                         nullptr);
            return crow::response(409, "Outlet Not Available");
        }
    }

    constexpr const char *query = "UPDATE OUTLET SET STATUS = 'occupied' WHERE ID = ?";
    Db::Stmt stmt(query, db);
    sqlite3_bind_int(stmt.get(), 1, outlet_id);
    if (sqlite3_step(stmt.get()) != SQLITE_DONE) {
        sqlite3_exec(db.get(), "end transaction", nullptr, nullptr, nullptr);
        return crow::response(
            500, "Internal Server Error -> Unable to Occupy Outlet");
    }
    sqlite3_exec(db.get(), "end transaction", nullptr, nullptr, nullptr);
    return crow::response(200, "Charge initiated successfully");
}
crow::response terminate_charge(const crow::request &req) {
    crow::json::rvalue body = crow::json::load(req.body);
    if (!body || !body.has("outlet_id")) {
        return crow::response(400, "Invalid request body");
    }
    const int user_id = Validate::get_userid_from_request(req);
    if (user_id == -1) {
        return crow::response(401, "Unauthorized");
    }
    const int outlet_id = body["outlet_id"].i();
    Db::Sqlite db(Db::DatabaseFile);
    constexpr const char *query = "UPDATE OUTLET SET STATUS = 'available' WHERE ID = ?";
    Db::Stmt stmt(query, db);
    sqlite3_bind_int(stmt.get(), 1, outlet_id);
    if (sqlite3_step(stmt.get()) != SQLITE_DONE) {
        return crow::response(500,
                              "Internal Server Error -> Unable to Free Outlet");
    }
    return crow::response(200, "Charge terminated successfully");
}
} // namespace Charging