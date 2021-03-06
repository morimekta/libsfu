#ifndef SFU_STRINGS_FORMAT_H_
#define SFU_STRINGS_FORMAT_H_

#include <string>
#include <cstdarg>

namespace sfu {
namespace strings {

const std::string format(const char* format, ...);
bool format_into(std::string* into, const char* format, ...);

}  // namespace strings
}  // namespace sfu

#endif  // SFU_STRINGS_FORMAT_H_
