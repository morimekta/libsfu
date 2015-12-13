#ifndef UTIL_TIME_FORMAT_H_
#define UTIL_TIME_FORMAT_H_

#include <string>
#include <cstdarg>

namespace sfu {
namespace time {

// Format reference:
// @url{http://www.cplusplus.com/reference/ctime/strftime/}
const std::string format(const char* format, time_t ts, bool utc = false);
bool format_into(std::string* into, const char* format, time_t ts, bool utc = false);

}  // namespace time
}  // namespace sfu

#endif
