#include <string>
#include <cstdarg>
#include <cstdio>
#include <ctime>

#include "sfu/time/format.h"

namespace sfu {
namespace time {
  
static const size_t kFormatStringTimeBufferSize = 64;

bool format_into(std::string* into, const char* fmt, time_t ts, bool utc) {
  char buffer[kFormatStringTimeBufferSize] = {0, };

  struct tm time;
  if (utc) {
    gmtime_r(static_cast<const time_t*>(&ts), &time);
  } else {
    localtime_r(static_cast<const time_t*>(&ts), &time);
  }
  size_t p = strftime(buffer, kFormatStringTimeBufferSize, fmt, &time);

  if (p >= kFormatStringTimeBufferSize) {
    return false;
  }
  into->clear();
  into->append(buffer, p);
  return true;
}

const std::string format(const char* fmt, time_t ts, bool utc) {
  std::string out("<error>");
  format_into(&out, fmt, ts, utc);
  return out;
}

}  // namespace strings
}  // namespace sfu
