#include "crow/http_request.h"
#include "crow/json.h"
#include "db/db.h"
#include "rest/charging.h"
#include "rest/management.h"
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
static void test_login_user(){
    crow::request req;
    crow::json::wvalue jbody;
    jbody["username"] = "testuser";
    jbody["password"] = "testpassword";
    req.body = jbody.dump();
    Management::login_user(req);
}
int main(){
    Db::DatabaseFile = "/workspaces/charging/charging-cpp/database/user.db";
    test_initiate_charge();
    test_terminate_charge();
    std::string username = "testuser";
    std::string password = "testpassword";
    long user_id = Validate::validate_user(username, password);
    test_login_user();
}