#include "utils/cache.h"
namespace Cache {
SessionInfo::SessionInfo(int uid, long long int oid)
    : user_id(uid), outlet_id(oid) {}
SessionInfo::SessionInfo() : user_id(-1), outlet_id(-1) {}
void create_session(const int user_id, const long outlet_id) {
    std::lock_guard<std::mutex> lock(session_mutex);
    session_map[user_id].insert(outlet_id);
    active_outlets.insert(outlet_id);
}
void delete_session(const int user_id, const long outlet_id) {
    std::lock_guard<std::mutex> lock(session_mutex);
    active_outlets.erase(outlet_id);
    session_map[user_id].erase(outlet_id);
}
bool session_exists(const int user_id, const long outlet_id) {
    std::lock_guard<std::mutex> lock(session_mutex);
    return session_map.find(user_id) != session_map.end() &&
           session_map[user_id].find(outlet_id) != session_map[user_id].end();
}
int outlet_status(long int outlet_id) {
    std::lock_guard<std::mutex> lock(session_mutex);
    return active_outlets.find(outlet_id) == active_outlets.end() ? AVAILABLE
                                                                  : OCCUPIED;
} 
} // namespace Cache