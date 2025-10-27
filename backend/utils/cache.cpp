#include "utils/cache.h"
namespace Cache {
SessionInfo::SessionInfo(int uid, long long int oid)
    : user_id(uid), outlet_id(oid) {}
SessionInfo::SessionInfo() : user_id(-1), outlet_id(-1) {}
void create_session(int user_id, long long int outlet_id) {
    std::lock_guard<std::mutex> lock(session_mutex);
    session_map[user_id] = outlet_id;
    active_outlets.insert(outlet_id);
}
void delete_session(int user_id) {
    std::lock_guard<std::mutex> lock(session_mutex);
    active_outlets.erase(session_map.at(user_id));
    session_map.erase(user_id);
}
bool session_exists(int user_id) {
    std::lock_guard<std::mutex> lock(session_mutex);
    return session_map.find(user_id) != session_map.end();
}
int outlet_status(long long int outlet_id) {
    std::lock_guard<std::mutex> lock(session_mutex);
    return active_outlets.find(outlet_id) == active_outlets.end() ? AVAILABLE : OCCUPIED;
} // namespace Cache
}