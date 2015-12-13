#include "sfu/strings/cord.h"

#include <algorithm>
#include <cstring>
#include <string>
#include <map>

namespace sfu {
namespace strings {

static const char *kEmptyString = "";
const size_t cord::npos = std::string::npos;

cord::cord() {
  ptr_ = kEmptyString;
  len_ = 0;
}

cord::cord(const char* ptr) {
  // assert(ptr != NULL);
  ptr_ = ptr;
  len_ = strlen(ptr);
}

cord::cord(const char* ptr, size_t len) {
  // assert(ptr != NULL && len >= 0);
  ptr_ = ptr;
  len_ = len == npos ? strlen(ptr_) : len;
}

cord::cord(const std::string& str) {
  ptr_ = str.c_str();
  len_ = str.size();
}

cord::cord(const std::string& str, size_t len) {
  // assert(len <= str.size());
  ptr_ = str.c_str();
  len_ = len == npos ? strlen(ptr_) : len;
}

cord::cord(const strings::cord& strp, size_t len) {
  // assert(len >= 0 && len <= strp.length());
  ptr_ = strp.ptr();
  len_ = len;
}

void cord::reset(const char* ptr) {
  // assert(ptr != NULL);
  ptr_ = ptr;
  len_ = strlen(ptr_);
}

void cord::reset(const std::string& str) {
  ptr_ = str.c_str();
  len_ = str.size();
}

size_t cord::find(char c) const {
  for (size_t i = 0; i < len_; ++i) {
    if (*(ptr_ + i) == c) return i;
  }
  return npos;
}

size_t cord::find_last(char c) const {
  for (size_t i = len_ - 1; i >= 0; ++i) {
    if (*(ptr_ + i) == c) return i;
  }
  return npos;
}

size_t cord::find(const strings::cord& str) const {
  // char -> last pos in search string;
  std::map<char, std::pair<size_t, size_t>> m;
  for (size_t i = 0; i < str.length(); ++i) {
    char c = str[i];
    auto p = m.find(c);
    if (p == m.end()) m[c] = std::pair<size_t, size_t>(i, i);
    else p->second.second = i;
  }
  size_t pos = 0;
  size_t last_spos = str.length() - 1;
  while (pos + str.length() <= len_) {
    char c = *(ptr_ + pos + last_spos);
    auto p = m.find(c);
    if (p == m.end()) {
      // char not in searched string.
      pos += str.length();
      continue;
    }

    if (p->second.second == last_spos) {
      // possible match.
      size_t j;
      for (j = 0; j < str.length(); ++j) {
        c = *(ptr_ + pos + j);
        if (str[j] != c) {
          p = m.find(c);
          if (p == m.end()) { pos += j + 1;
          } else {            pos += j + p->second.first; }
          break;
        }

        if (j == last_spos) {
          return pos;
        }
      }
    } else {
      pos += (str.length() - p->second.second);
    }
  }
  return npos;
}


bool cord::string_equals(const strings::cord& other) const {
  if (len_ != other.length()) return false;
  return strncmp(ptr_, other.ptr(), len_) == 0;
}

bool cord::string_less_than(const strings::cord& other) const {
  int i = strncmp(ptr_, other.ptr(), std::min(len_, other.length()));
  if (i == 0) return len_ < other.length();
  return i < 0;
}

const std::string cord::as_string() const {
  return std::string(ptr_, len_);
}

}  // namespace strings
}  // namespace sfu

namespace std {
  template<> void swap(sfu::strings::cord& lhs, sfu::strings::cord& rhs) {
    const char* lhs_ptr = lhs.ptr();
    size_t lhs_len = lhs.length();
    lhs.reset(rhs);
    rhs.reset(lhs_ptr, lhs_len);
  };
}

std::ostream& operator<<(std::ostream& out,
                         const sfu::strings::cord& sp) {
  for (size_t i = 0; i < sp.length(); ++i) {
    out << sp[i];
  }
  return out;
}
