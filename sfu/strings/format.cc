#include <string>
#include <cstdarg>
#include <cstdio>
#include <ctime>

#include "sfu/strings/format.h"

namespace sfu {
namespace strings {

static const size_t kFormatStringBufferSize = 512;

const std::string format(const char* fmt, ...) {
  char buffer[kFormatStringBufferSize] = {0, };
  va_list ap;
  va_start(ap, fmt);
  size_t p = vsnprintf(buffer, kFormatStringBufferSize, fmt, ap);
  va_end(ap);
  if (p >= kFormatStringBufferSize) {
    p = kFormatStringBufferSize - 1;
  }
  return std::string(buffer, p);
}

bool format_into(std::string* into, const char* fmt, ...) {
  char buffer[kFormatStringBufferSize] = {0, };
  va_list ap;
  va_start(ap, fmt);
  size_t x = vsnprintf(buffer, kFormatStringBufferSize, fmt, ap);
  va_end(ap);
  if (x >= kFormatStringBufferSize) {
    return false;
  }
  into->clear();
  into->append(buffer, std::min(x, kFormatStringBufferSize - 1));
  return true;
}

}  // namespace strings
}  // namespace sfu
