#include "crow/app.h"
#include "crow/http_response.h"
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
        .methods(crow::HTTPMethod::POST)(Management::get_outlets);
    CROW_ROUTE(app, "/api/charge/initiate")
        .methods(crow::HTTPMethod::POST)(Charging::initiate_charge);
    CROW_ROUTE(app, "/api/charge/terminate")
        .methods(crow::HTTPMethod::POST)(Charging::terminate_charge);
    CROW_ROUTE(app, "/embd/outlet/status")
        .methods(crow::HTTPMethod::GET)(Charging::get_outlet_status);
    CROW_ROUTE(app, "/").methods(crow::HTTPMethod::GET)([](){
        crow::response res;
        res.code = 200;
        res.set_static_file_info("frontend/index.html");
        return res;
    });
    CROW_ROUTE(app, "/<path>")
        .methods(crow::HTTPMethod::GET)([](const crow::request &req,
                                          const std::string &path) {
            if(path.find("..") != std::string::npos) {
                return crow::response(400, "Bad Request");
            }
            crow::response res;
            res.code = 200;
            res.set_static_file_info("frontend/" + path);
            return res;
        });
    app.port(8080).multithreaded().run();
}
int main() {
    Crypt::init();
    run();
    Charging::free_outlets();
    return 0;
}
