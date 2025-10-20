#include "rest/charge.h"
#include "db/db.h"
namespace Charge {
    Station::Station(std::string_view name, std::string_view location)
        : id(-1), name(name), location(location) {}
    Outlet::Outlet(int station_id, std::string_view name, std::string_view status)
        : id(-1), station_id(station_id), name(name), status(status) {}
    User::User(std::string_view name, std::string_view email, std::string_view password)
        : id(-1), name(name), email(email), password(password) {}

    
    crow::response add_station(const crow::request &req){

    }
}

