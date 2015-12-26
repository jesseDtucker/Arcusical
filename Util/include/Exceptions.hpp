#include "boost/exception/all.hpp"

namespace Util {
typedef boost::error_info<struct errinfo_msg_, std::string> errinfo_msg;
struct NoSongFileAvailable : virtual boost::exception {};
}