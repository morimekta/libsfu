#include "sfu/strings/strings.h"

#include <cctype>
#include <iostream>
#include <cstring>

using namespace std;

namespace sfu {
namespace strings {

size_t split_cb(function<bool(const string&)> out,
                const string& str,
                const string& sep,
                bool omit_empty) {
  size_t lastsep = 0, isep = 0;
  int i = 0;
  while (lastsep < str.size() &&
         (isep = str.find(sep, lastsep)) < str.size()) {
    string tmp(str.substr(lastsep, isep - lastsep));
    if (!omit_empty || tmp.size() > 0) {
      if (!out(tmp)) return string::npos;
      ++i;
    }
    lastsep = isep + sep.size();
  }

  if (!omit_empty || lastsep < (str.size() - 1)) {
    if (!out(str.substr(lastsep))) return string::npos;
    ++i;
  }
  return i;
}

size_t split_kvp_cb(function<bool(const string&,
                                  const string&)> out,
                    const string& str,
                    const string& pair_sep,
                    const string& kv_sep,
                    bool omit_empty_pairs,
                    bool omit_empty_values,
                    bool allow_no_sep) {
  size_t ret = 0;
  list<string> tmp;
  split_into(&tmp, str, pair_sep, omit_empty_pairs);
  for (string& entry : tmp) {
    if (entry.empty()) {
      if (omit_empty_pairs) continue;
      return string::npos;
    }

    size_t sep = entry.find(kv_sep);
    if (sep == 0) {
      // empty key...
      return string::npos;
    }
    string value = "";
    if (sep == string::npos) {
      if (!allow_no_sep) {
        return string::npos;
      }
    } else {
      value = entry.substr(sep + kv_sep.size());
    }

    string key(entry.substr(0, sep));
    if (value.empty() && omit_empty_values) {
      continue;
    }

    if (!out(key, value)) {
      return string::npos;
    }
    ++ret;
  }

  return ret;
}

std::map<std::string, std::string> split_kvp(
    const std::string& str,
    const std::string& pair_sep,
    const std::string& kv_sep,
    bool omit_empty_pairs,
    bool omit_empty_values,
    bool allow_no_sep) {
  std::map<std::string, std::string> out;
  split_kvp_into(&out, str, pair_sep, kv_sep,
      omit_empty_pairs, omit_empty_values, allow_no_sep);
  return out;
}

// ...

bool has_prefix(const std::string& str, const std::string& prefix) {
  if (prefix.size() > str.size()) return false;
  for (size_t i = 0; i < prefix.size(); ++i) {
    if (str[i] != prefix[i]) return false;
  }
  return true;
}

bool has_suffix(const std::string& str, const std::string& suffix) {
  if (suffix.size() > str.size()) return false;
  size_t suffix_start = str.size() - suffix.size();
  for (size_t i = 0; i < suffix.size(); ++i) {
    if (str[suffix_start + i] != suffix[i]) return false;
  }
  return true;
}

bool strip_prefix(std::string* str, const std::string& prefix) {
  if (has_prefix(*str, prefix)) {
    *str = str->substr(prefix.size());
    return true;
  }
  return false;
}

bool strip_suffix(std::string* str, const std::string& suffix) {
  if (has_suffix(*str, suffix)) {
    *str = str->substr(0, str->size() - suffix.size());
    return true;
  }
  return false;
}

size_t trim(std::string* str, const std::function<bool(char)>& f) {
  size_t ret = 0, begin = 0, end = str->size();

  while (begin < end && f((*str)[begin])) {
    ++begin;
    ++ret;
  }
  while (begin < end && f((*str)[end - 1])) {
    --end;
    ++ret;
  }
  if (ret > 0) {
    *str = str->substr(begin, end - begin);
    return ret;
  }
  return 0;
}

size_t trim_left(std::string* str, const std::function<bool(char)>& f) {
  size_t begin = 0, end = str->size();

  while (begin < end && f((*str)[begin])) {
    ++begin;
  }
  if (begin > 0) {
    *str = str->substr(begin, end - begin);
  }
  return begin;
}

size_t trim_right(std::string* str, const std::function<bool(char)>& f) {
  size_t ret = 0, end = str->size();

  while (0 < end && f((*str)[end - 1])) {
    --end;
    ++ret;
  }
  if (ret > 0) {
    *str = str->substr(0, end);
  }
  return ret;
}

size_t trim_whitespace(std::string* str) {
  size_t ret = 0, begin = 0, end = str->size();

  while (begin < end && (isblank((*str)[begin]) ||
                         isspace((*str)[begin]))) {
    ++begin;
    ++ret;
  }
  while (begin < end && (isblank((*str)[end - 1]) ||
                         isspace((*str)[end - 1]))) {
    --end;
    ++ret;
  }
  if (ret > 0) {
    *str = str->substr(begin, end - begin);
  }
  return ret;
}

}  // namespace strings
}  // namespace sfu
