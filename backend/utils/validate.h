#pragma once
#include "crow/http_request.h"
#include <string>
namespace Validate {
long validate_user(const std::string &username, const std::string &password);
long get_userid_from_request(const crow::request &req);
} // namespace Validate