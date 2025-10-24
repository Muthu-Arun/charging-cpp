#include "crow/http_request.h"
#include "crow/http_response.h"
#include "crow/json.h"
#include "rest/charging.h"
int main(){
    crow::request req;
    crow::json::wvalue body;
    body["outlet_id"] = 1;
    req.method = crow::HTTPMethod::GET;
    req.body = body.dump();
    auto res = Charging::get_outlet_status(req);
}