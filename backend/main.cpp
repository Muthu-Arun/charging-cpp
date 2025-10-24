#include "crow/app.h"
#include "rest/management.h"
#include "rest/charging.h"
void run() {
    crow::SimpleApp app;
    CROW_ROUTE(app, "/app/health").methods(crow::HTTPMethod::GET)([]() {
        return crow::response(200, "Service is Healthy");
    });
    CROW_ROUTE(app, "/admin/add_outlet")
        .methods(crow::HTTPMethod::POST)(Management::add_outlet);
    CROW_ROUTE(app, "/admin/add_station")
        .methods(crow::HTTPMethod::POST)(Management::add_station);
    CROW_ROUTE(app, "/login/register")
        .methods(crow::HTTPMethod::POST)(Management::register_user);
    CROW_ROUTE(app, "/login/login")
        .methods(crow::HTTPMethod::POST)(Management::login_user);
    CROW_ROUTE(app, "/api/stations")
        .methods(crow::HTTPMethod::GET)(Management::get_stations);
    CROW_ROUTE(app, "/api/outlets")
        .methods(crow::HTTPMethod::GET)(Management::get_outlets);
    CROW_ROUTE(app, "/api/charge/initiate")
        .methods(crow::HTTPMethod::POST)(Charging::initiate_charge);
    CROW_ROUTE(app, "/api/charge/terminate")
        .methods(crow::HTTPMethod::POST)(Charging::terminate_charge);
    CROW_ROUTE(app, "/embd/outlet/status")
        .methods(crow::HTTPMethod::GET)(Charging::get_outlet_status);
    app.port(8080).multithreaded().run();
}
int main() {
    run();
    return 0;
}
