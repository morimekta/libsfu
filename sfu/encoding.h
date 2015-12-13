#ifndef UTIL_ENCODING_H_
#define UTIL_ENCODING_H_

#include <string>

#include "sfu/strings/cord.h"

namespace sfu {

// Short hand to escape strings in the format parsed by C compilers for char[]
// strings.
const std::string CEscape(const strings::cord& str);

void CEncode(const strings::cord &str, std::string *out);
bool CDecode(const strings::cord &str, std::string *out);

// Encode data as a continous string of hex
void HexEncode(const strings::cord& str, std::string *hex);
bool HexDecode(const strings::cord& hex, std::string *str);

void UrlEncode(const strings::cord& str, std::string *encoded);
bool UrlDecode(const strings::cord& encoded, std::string *str);

void Base64Encode(const strings::cord& data, bool web_safe, std::string *base64);
bool Base64Decode(const strings::cord& base64, bool web_safe, std::string *data);

}  // namespace sfu

#endif  // UTIL_ENCODING_H_
