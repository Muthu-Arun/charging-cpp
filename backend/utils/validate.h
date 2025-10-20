#pragma once
#include "crow/http_request.h"
#include <string>
#include "utils/hash.h"
namespace Validate {
long validate_user(const std::string &username, const std::string &password);
long get_userid_from_request(const crow::request &req);
} // namespace Validate