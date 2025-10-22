#include "crow/http_response.h"
#include "crow/json.h"
#include "utils/validate.h"
#include <map>
namespace Charging {
crow::response initiate_charge(const crow::request &req);
crow::response terminate_charge(const crow::request &req); 
} // namespace Charging