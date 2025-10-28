#include <map>
#include <mutex>
#include <set>

namespace Cache {
struct SessionInfo {
    int user_id;
    long long int outlet_id;
    SessionInfo();
    SessionInfo(SessionInfo &other) = default;
    SessionInfo(SessionInfo &&other) noexcept = delete;
    SessionInfo(int uid, long long int oid);
};
inline std::set<int> active_outlets;
inline std::map<int, std::set<long>> session_map;
inline std::mutex session_mutex;
enum OutletStatus { OCCUPIED = 0, AVAILABLE = 1 };
void create_session(int user_id, long int outlet_id);
void delete_session(int user_id, long int outlet_id);
bool session_exists(int user_id, long int outlet_id);
int outlet_status(long int outlet_id);
} // namespace Cache