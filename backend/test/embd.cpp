#include "crow/app.h"
#include "crow/common.h"
#include "crow/http_request.h"
#include "crow/http_response.h"
#include "crow/json.h"
#include "rest/charging.h"
#include "utils/cache.h"
#include <string>
/*
int main(){
    crow::request req;
    crow::json::wvalue body;
    body["outlet_id"] = 1;
    req.method = crow::HTTPMethod::GET;
    req.body = body.dump();
    auto res = Charging::get_outlet_status(req);
}
*/
/*
int main() {
    Cache::create_session(1, 1);
    if (Cache::session_exists(1)) {
        Cache::delete_session(1);
    }
    return 0;

}
*/
static void route(crow::SimpleApp &app) {
    CROW_ROUTE(app, "/api/charge/initiate")
        .methods(crow::HTTPMethod::POST)([](const crow::request &req) {
            crow::json::rvalue body = crow::json::load(req.body);
            if (!body || !body.has("user_id") || !body.has("outlet_id")) {
                return crow::response(400, "Invalid request body");
            }
            const int user_id = body["user_id"].i();
            const long long int outlet_id = body["outlet_id"].i();
            if (Cache::outlet_status(outlet_id) == Cache::OCCUPIED) {
                return crow::response(409, "Outlet is already occupied");
            } else {
                Cache::create_session(user_id, outlet_id);
                return crow::response(crow::OK, "Charging session initiated");
            }
        });
    CROW_ROUTE(app, "/api/charge/terminate")
        .methods(crow::HTTPMethod::POST)([](const crow::request &req) {
            crow::json::rvalue body = crow::json::load(req.body);
            if (!body || !body.has("user_id")) {
                return crow::response(400, "Invalid request body");
            }
            const int user_id = body["user_id"].i();
            if (!Cache::session_exists(user_id)) {
                return crow::response(404, "No active session for user");
            } else {
                Cache::delete_session(user_id);
                return crow::response(crow::OK, "Charging session terminated");
            }
        });
    CROW_ROUTE(app, "/embd/outlet/status")
        .methods(crow::HTTPMethod::GET)([](const crow::request &req) {
            crow::json::rvalue body = crow::json::load(req.body);
            if (!body || !body.has("outlet_id")) {
                return crow::response(400, "Invalid request body");
            }
            const long long int outlet_id = body["outlet_id"].i();
            crow::json::wvalue res_body;
            res_body["outlet_id"] = outlet_id;
            res_body["status"] = Cache::OutletStatus(outlet_id);
            return crow::response(crow::OK, res_body);
        });
}

int main() { crow::SimpleApp app; }