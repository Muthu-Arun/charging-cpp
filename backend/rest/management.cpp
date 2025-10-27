#include "rest/management.h"
#include "crow/http_response.h"
#include "crow/json.h"
#include "db/db.h"
#include "utils/validate.h"
#include <iostream>
namespace Management {
Station::Station(std::string_view name, std::string_view location)
    : id(-1), name(name), location(location) {}
Outlet::Outlet(int station_id, std::string_view name, std::string_view status)
    : id(-1), station_id(station_id), name(name), status(status) {}
User::User(std::string_view name, std::string_view email,
           std::string_view password)
    : id(-1), name(name), email(email), password(password) {}

crow::response add_station(const crow::request &req) {
    crow::json::rvalue body = crow::json::load(req.body);
    if (!body || !body.has("name") || !body.has("location")) {
        return crow::response(400, "Invalid request body");
    }
    const std::string name = body["name"].s();
    const std::string location = body["location"].s();
    constexpr const char *query =
        "INSERT INTO station (name, location) VALUES (?, ?)";
    Db::Sqlite db(Db::DatabaseFile);
    Db::Stmt stmt(query, db);
    sqlite3_bind_text(stmt.get(), 1, name.c_str(), -1, nullptr);
    sqlite3_bind_text(stmt.get(), 2, location.c_str(), -1, nullptr);
    if (sqlite3_step(stmt.get()) != SQLITE_DONE) {
        return crow::response(500, "Failed to add station");
    }

    return crow::response(201, "Station added successfully");
}
crow::response add_outlet(const crow::request &req) {
    crow::json::rvalue body = crow::json::load(req.body);
    if (!body || !body.has("station_id") || !body.has("name")) {
        return crow::response(400, "Invalid request body");
    }
    const int station_id = body["station_id"].i();
    const std::string name = body["name"].s();
    const std::string status = "available";
    constexpr const char *query =
        "INSERT INTO outlet (station_id, name, status) VALUES (?, ?, ?)";
    Db::Sqlite db(Db::DatabaseFile);
    Db::Stmt stmt(query, db);
    sqlite3_bind_int(stmt.get(), 1, station_id);
    sqlite3_bind_text(stmt.get(), 2, name.c_str(), -1, nullptr);
    sqlite3_bind_text(stmt.get(), 3, status.c_str(), -1, nullptr);
    if (sqlite3_step(stmt.get()) != SQLITE_DONE) {
        return crow::response(500, "Failed to add outlet");
    }
    return crow::response(201, "Outlet added successfully");
}
crow::response register_user(const crow::request &req) {
    crow::json::rvalue body = crow::json::load(req.body);
    if (!body || !body.has("name") || !body.has("email") ||
        !body.has("password")) {
        return crow::response(400, "Invalid request body");
    }
    const std::string name = body["name"].s();
    const std::string email = body["email"].s();
    const std::string password = body["password"].s();
    Crypt::hash_password(Crypt::hashed_password, password.c_str());
    constexpr const char *query =
        "INSERT INTO user (name, email, password) VALUES (?, ?, ?)";
    Db::Sqlite db(Db::DatabaseFile);
    Db::Stmt stmt(query, db);
    sqlite3_bind_text(stmt.get(), 1, name.c_str(), -1, nullptr);
    sqlite3_bind_text(stmt.get(), 2, email.c_str(), -1, nullptr);
    sqlite3_bind_text(stmt.get(), 3, Crypt::hashed_password, -1, nullptr);
    if (sqlite3_step(stmt.get()) != SQLITE_DONE) {
        return crow::response(500, "Failed to add user");
    }
    return crow::response(201, "User added successfully");
}
crow::response login_user(const crow::request &req) {
    std::cout << "Debug: login_user called" << std::endl;
    crow::json::rvalue body = crow::json::load(req.body);
    if (!body || !body.has("username") || !body.has("password")) {
        return crow::response(crow::status::BAD_REQUEST,
                              "Invalid request body.");
    }

    const std::string username = body["username"].s();
    const std::string password = body["password"].s();

    // In a real app, you would validate username and password against a
    // database. For this example, we'll use a simple check.
    std::cout << "Debug: Validating user " << username << std::endl;
    long user_id = Validate::validate_user(username, password);
    std::cout << "Debug: validate_user returned user_id " << user_id << std::endl;
    if (user_id != -1) {
        // Create the JWT
        auto token =
            jwt::create()
                .set_issuer("bit-app")
                .set_subject(username)
                .set_issued_at(std::chrono::system_clock::now())
                .set_expires_at(std::chrono::system_clock::now() +
                                std::chrono::days(30))
                .set_payload_claim(
                    "user_id", jwt::claim(std::string(std::to_string(user_id))))
                .sign(jwt::algorithm::hs256{Validate::JWT_SECRET});

        // Return the token to the client
        crow::json::wvalue result;
        result["token"] = token;
        return crow::response(crow::status::OK, result);
    }

    return crow::response(crow::status::UNAUTHORIZED, "Invalid credentials.");
}
crow::response get_stations(const crow::request &req) {
    constexpr const char *query = "SELECT id, name, location FROM station";
    Db::Sqlite db(Db::DatabaseFile);
    Db::Stmt stmt(query, db);
    crow::json::wvalue::list result_list;
    crow::json::wvalue result;
    while (sqlite3_step(stmt.get()) == SQLITE_ROW) {
        crow::json::wvalue station;
        station["id"] = sqlite3_column_int(stmt.get(), 0);
        station["name"] =
            reinterpret_cast<const char *>(sqlite3_column_text(stmt.get(), 1));
        station["location"] =
            reinterpret_cast<const char *>(sqlite3_column_text(stmt.get(), 2));
        result_list.push_back(station);
    }
    result["stations"] = std::move(result_list);
    return crow::response(200, result);
}
crow::response get_outlets(const crow::request &req) {
    std::cout << "Debug: Executing get_outlets for station_id " << std::endl;
    crow::json::rvalue body = crow::json::load(req.body);
    if (!body || !body.has("station_id")) {
        return crow::response(400, "Invalid request body");
    
    }
    const long station_id = body["station_id"].i();
     
    static const char *query =
        "SELECT id, station_id, name, status FROM outlet WHERE station_id = ?";
    Db::Sqlite db(Db::DatabaseFile);
    Db::Stmt stmt(query, db);
    sqlite3_bind_int64(stmt.get(), 1, station_id);
    crow::json::wvalue::list result_list;
    crow::json::wvalue result;
    while (sqlite3_step(stmt.get()) == SQLITE_ROW) {
        crow::json::wvalue outlet;
        outlet["id"] = sqlite3_column_int(stmt.get(), 0);
        outlet["station_id"] = sqlite3_column_int(stmt.get(), 1);
        outlet["name"] =
            reinterpret_cast<const char *>(sqlite3_column_text(stmt.get(), 2));
        outlet["status"] =
            reinterpret_cast<const char *>(sqlite3_column_text(stmt.get(), 3));
        result_list.push_back(outlet);
        std::cout << "Debug: Fetched outlet " <<outlet.dump() << std::endl;
    }
    result["outlets"] = std::move(result_list);
    return crow::response(200, result);
}
crow::response health_check(const crow::request &req) {
    return crow::response(200, "Service is healthy");
}

} // namespace Management
