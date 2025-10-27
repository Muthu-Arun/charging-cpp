#include "crow/http_request.h"
#include "crow/json.h"
#include "rest/charging.h"
static void test_initiate_charge(){
    crow::request req;
    crow::json::wvalue jbody;
    jbody["outlet_id"] = 1;
    req.body = jbody.dump();
    Charging::initiate_charge(req);
    

}
static void test_terminate_charge(){
    crow::request req;
    crow::json::wvalue jbody;
    jbody["outlet_id"] = 1;
    req.body = jbody.dump();
    Charging::terminate_charge(req);
    

}
int main(){
    test_initiate_charge();
    test_terminate_charge();

}