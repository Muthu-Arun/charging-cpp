#include "rest/charging.h"
#include "crow/common.h"
#include "crow/http_request.h"
#include "crow/http_response.h"
#include "db/db.h"
#include <string>

namespace Charging {
crow::response initiate_charge(const crow::request &req) {
    crow::json::rvalue body = crow::json::load(req.body);
    if (!body || !body.has("outlet_id")) {
        return crow::response(crow::BAD_REQUEST, "Invalid request body");
    }
    const int user_id = Validate::get_userid_from_request(req);
    // const int user_id = 1;
    if (user_id == -1) {
        return crow::response(crow::UNAUTHORIZED, "Unauthorized");
    }
    const long int outlet_id = body["outlet_id"].i();
    Db::Sqlite db(Db::DatabaseFile);
    // Checking the availability of the outlet and occupying it must be in a
    // single transaction
    sqlite3_exec(db.get(), "BEGIN TRANSACTION", nullptr, nullptr, nullptr);
    {
        static const char *query = "SELECT STATUS FROM OUTLET WHERE ID = ?";
        Db::Stmt stmt(query, db);
        sqlite3_bind_int64(stmt.get(), 1, outlet_id);
        if (sqlite3_step(stmt.get()) != SQLITE_ROW) {
            sqlite3_exec(db.get(), "ROLLBACK", nullptr, nullptr, nullptr);
            return crow::response(
                crow::INTERNAL_SERVER_ERROR,
                "Internal Server Error -> Unable to Check Outlet Availability");
        }
        const unsigned char *availability = sqlite3_column_text(stmt.get(), 0);
        if (std::string(availability
                            ? reinterpret_cast<const char *>(availability)
                            : "") != "available") {
            sqlite3_exec(db.get(), "", nullptr, nullptr, nullptr);
            return crow::response(crow::CONFLICT, "Outlet Not Available");
        }
    }

    static const char *query =
        "UPDATE OUTLET SET STATUS = 'occupied' WHERE ID = ?";
    Db::Stmt stmt(query, db);
    sqlite3_bind_int(stmt.get(), 1, outlet_id);
    if (sqlite3_step(stmt.get()) == SQLITE_DONE) {
        sqlite3_exec(db.get(), "END TRANSACTION", nullptr, nullptr, nullptr);
        return crow::response(crow::OK, "Charge initiated successfully");
    }
    sqlite3_exec(db.get(), "END TRANSACTION", nullptr, nullptr, nullptr);
    return crow::response(500,
                          "Internal Server Error -> Unable to Occupy Outlet");
}
crow::response terminate_charge(const crow::request &req) {
    crow::json::rvalue body = crow::json::load(req.body);
    if (!body || !body.has("outlet_id")) {
        return crow::response(crow::BAD_REQUEST, "Invalid request body");
    }
    const int user_id = Validate::get_userid_from_request(req);
    // const int user_id = 1;
    if (user_id == -1) {
        return crow::response(crow::UNAUTHORIZED, "Unauthorized Request");
    }
    const long int outlet_id = body["outlet_id"].i();
    Db::Sqlite db(Db::DatabaseFile);
    static const char *query =
        "UPDATE OUTLET SET STATUS = 'available' WHERE ID = ?";
    Db::Stmt stmt(query, db);
    sqlite3_bind_int64(stmt.get(), 1, outlet_id);
    if (sqlite3_step(stmt.get()) != SQLITE_DONE) {
        return crow::response(crow::INTERNAL_SERVER_ERROR,
                              "Internal Server Error -> Unable to Free Outlet");
    }
    return crow::response(crow::OK, "Charging terminated successfully");
}
crow::response get_outlet_status(const crow::request &req) {
    crow::json::rvalue body = crow::json::load(req.body);
    if (!body || !body.has("outlet_id")) {
        return crow::response(crow::BAD_REQUEST, "Invalid request body");
    }
    // Need to Implement Authentication For Embedded Systems Later
    const long int outlet_id = body["outlet_id"].i();
    Db::Sqlite db(Db::DatabaseFile);
    static const char *query = "SELECT STATUS FROM OUTLET WHERE ID = ?";
    Db::Stmt stmt(query, db);
    sqlite3_bind_int64(stmt.get(), 1, outlet_id);
    if (sqlite3_step(stmt.get()) != SQLITE_ROW) {
        return crow::response(
            crow::INTERNAL_SERVER_ERROR,
            "Internal Server Error -> Unable to Fetch Outlet Status");
    }
    const unsigned char *status = sqlite3_column_text(stmt.get(), 0);
    crow::json::wvalue result;
    result["status"] =
        status ? reinterpret_cast<const char *>(status) : "unknown";
    return crow::response(crow::OK, result);
}
} // namespace Charging