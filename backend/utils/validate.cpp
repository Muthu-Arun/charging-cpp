#include "utils/validate.h"
#include "db/db.h"
#include "jwt-cpp/jwt.h"
#include "utils/hash.h"
static const std::string JWT_SECRET = std::getenv("JWT_SECRET")
                                          ? std::getenv("JWT_SECRET")
                                          : "TheSecretKeyInDevlopment";
namespace Validate {
long get_userid_from_request(const crow::request &req) {
    auto authorization_header = req.get_header_value("Authorization");
    if (authorization_header.empty() ||
        authorization_header.find("Bearer ") != 0) {
        return -1; // Missing or invalid Authorization header
    }
    std::string token = authorization_header.substr(7); // Remove "Bearer "
    try {
        auto decoded = jwt::decode(token);
        auto verifier = jwt::verify()
                            .allow_algorithm(jwt::algorithm::hs256{JWT_SECRET})
                            .with_issuer("bit-app");
        verifier.verify(decoded);
        auto user_id_claim = decoded.get_payload_claim("user_id");
        return std::stol(user_id_claim.as_string());
    } catch (const std::exception &e) {
        return -1; // Invalid token
    }
}
long validate_user(const std::string &username, const std::string &password) {
    // Validate and return user ID if valid, else return -1

    Db::Sqlite db(Db::DatabaseFile);
    Db::Stmt stmt;
    constexpr static const char *query_email =
        "SELECT id, password FROM user where email = ?";
    constexpr static const char *query =
        "SELECT id, password FROM user WHERE username = ?"; // AND password =
                                                            // ?";
    if (username.find('@') == std::string::npos)
        stmt = Db::Stmt(query, db);
    else
        stmt = Db::Stmt(query_email, db);
    sqlite3_bind_text(stmt.get(), 1, username.c_str(), -1, nullptr);
    int rc = sqlite3_step(stmt.get());
    if (rc == SQLITE_ROW) {
        long user_id = sqlite3_column_int64(stmt.get(), 0); // Assuming ID is
                                                            // the first column
        const char *actual_password =
            reinterpret_cast<const char *>(sqlite3_column_text(stmt.get(), 1));

        if (!Crypt::verify_hash(actual_password, password.c_str())) {
            return -1;
        }
        return user_id;
    } else {
        return -1; // Invalid credentials
    }
}
} // namespace Validate