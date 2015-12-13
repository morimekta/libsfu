#include "sfu/strings/cord.h"

#include <algorithm>
#include <string>
#include <cmath>

using namespace sfu;

namespace sfu {
namespace {

template<typename T>
inline bool safe_parse_int(const strings::cord& str, T* num, int base) {
  bool neg = false;
  if (base < 2 || base > 36) {
    return false;
  }
  if (str.length() == 0) return false;
  if (str[0] == '-') neg = true;
  if (neg && str.length() == 1) return false;
  T val = 0;
  for (size_t i = (neg ? 1 : 0); i < str.length(); ++i) {
    char c = str[i];
    int tmp = 0;
    if (c >= '0' && c <= '9') {
      tmp = c - '0';
    } else if (c >= 'a' && c <= 'z') {
      tmp = c + 10 - 'a';
    } else if (c >= 'A' && c <= 'Z') {
      tmp = c + 10 - 'A';
    } else {
      return false;
    }
    if (tmp >= base) return false;

    val *= base;
    val += tmp;
  }

  (*num) = neg ? -val : val;
  return true;
}

template<typename T>
inline bool safe_parse_uint(const strings::cord& str, T* num, int base) {
  if (base < 2 || base > 36) {
    return false;
  }
  if (str.length() == 0) return false;
  T val = 0;
  for (size_t i = 0; i < str.length(); ++i) {
    char c = str[i];
    int tmp = 0;
    if (c >= '0' && c <= '9') {
      tmp = c - '0';
    } else if (c >= 'a' && c <= 'z') {
      tmp = c + 10 - 'a';
    } else if (c >= 'A' && c <= 'Z') {
      tmp = c + 10 - 'A';
    } else {
      return false;
    }
    if (tmp >= base) return false;

    val *= base;
    val += tmp;
  }

  (*num) = val;
  return true;
}

template<typename T>
inline bool safe_parse_float(const strings::cord& str, T* dbl) {
  strings::cord strp(str);
  if (str.length() == 0) return false;
  bool neg = false;
  if (str[0] == '-') {
    strp.reset(str.ptr() + 1, str.length() - 1);
    neg = true;
  }
  size_t pt = strp.find('.');

  T result = 0;
  if (pt != strings::cord::npos) {
    strings::cord decimal(strp.ptr(), pt);
    strings::cord fraction(strp.ptr() + pt + 1, strp.length() - pt - 1);

    uint64_t dec = 0;
    uint64_t f = 0;
    if (decimal.length() > 0 && !safe_parse_uint(decimal, &dec, 10)) return false;
    if (fraction.length() > 0 && !safe_parse_uint(fraction, &f, 10)) return false;

    result = (static_cast<T>(dec) + (static_cast<T>(f) / std::pow(10, fraction.length())));
  }
  (*dbl) = neg ? -result : result;
  return true;
}

}  // namespace


bool safe_str2bool(const strings::cord& str, bool* value) {
  std::string val(str.ptr(), str.length());
  std::transform(val.begin(), val.end(), val.begin(), ::tolower);
  // Name don't matter now.
  if (val == "1" || val == "true" || val == "yes" || val == "y" || val == "t") {
    *value = true;
    return true;
  } else if (val == "0" || val == "false" || val == "no" || val == "n" || val == "f") {
    *value = false;
    return true;
  }

  return false;
}

bool safe_str2int8(const strings::cord& str, int8_t *num, int base) {
  return safe_parse_int(str, num, base);
}

bool safe_str2uint8(const strings::cord& str, uint8_t *num, int base) {
  return safe_parse_uint(str, num, base);
}

bool safe_str2int16(const strings::cord& str, int16_t *num, int base) {
  return safe_parse_int(str, num, base);
}

bool safe_str2uint16(const strings::cord& str, uint16_t *num, int base) {
  return safe_parse_uint(str, num, base);
}

bool safe_str2int32(const strings::cord& str, int32_t *num, int base) {
  return safe_parse_int(str, num, base);
}

bool safe_str2uint32(const strings::cord& str, uint32_t *num, int base) {
  return safe_parse_uint(str, num, base);
}

bool safe_str2int64(const strings::cord& str, int64_t *num, int base) {
  return safe_parse_int(str, num, base);
}

bool safe_str2uint64(const strings::cord& str, uint64_t *num, int base) {
  return safe_parse_uint(str, num, base);
}

bool safe_str2f(const strings::cord& str, float* dbl) {
  return safe_parse_float(str, dbl);
}

bool safe_str2d(const strings::cord& str, double* dbl) {
  return safe_parse_float(str, dbl);
}

bool safe_str2ld(const strings::cord& str, long double* dbl) {
  return safe_parse_float(str, dbl);
}

}  // namespace sfu
