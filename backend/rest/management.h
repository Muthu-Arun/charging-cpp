#pragma once
#include "crow/http_request.h"
#include "crow/http_response.h"
#include <string>
#include "utils/validate.h"
#include "jwt-cpp/jwt.h"
#include <string_view>
namespace Management {
struct Station {
    int id;
    std::string name, location;
    Station(std::string_view, std::string_view);
};
struct Outlet {
    int id, station_id;
    std::string name, status;
    Outlet(int, std::string_view, std::string_view);
};
struct User {
    int id;
    std::string name, email, password;
    User(std::string_view, std::string_view, std::string_view);
};
crow::response add_station(const crow::request &req);
crow::response add_outlet(const crow::request &req);
crow::response register_user(const crow::request &req);
crow::response login_user(const crow::request &req);
crow::response get_stations(const crow::request &req);
crow::response get_outlets(const crow::request &req);
crow::response initiate_charging(const crow::request &req);

} // namespace Management