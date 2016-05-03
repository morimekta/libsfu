#ifndef SFU_STRINGS_H_
#define SFU_STRINGS_H_

#include <functional>
#include <list>
#include <vector>
#include <string>
#include <map>
#include <iterator>

#include "sfu/container_traits.h"

namespace sfu {
namespace strings {

// split     (string, string) -> vector<string>
// split_into(Container*, string, string) -> size_t
// split_cb  (bool(string), string, string)) -> size_t
//
// join      (iterator, iterator, string) -> string
// join_into (iterator, iterator, string, string*) -> size_t
//
// split_kvp     (string, string, string) -> map<string, string>
// split_kvp_into(AssociativeContainer*, string, string, string) -> size_t
// split_kvp_cb  (bool(string, string), string, string, string) -> size_t

typedef std::iterator<std::input_iterator_tag, std::string> InputIterator;
// typedef Container std::
// typedef AssociativeContainer 

size_t split_cb(std::function<bool(const std::string&)> out,
                const std::string& str,
                const std::string& sep,
                bool omit_empty = false);

// Container must be a modifiable stl container.
template<class Container> inline
size_t split_into(Container* out,
                  const std::string& str,
                  const std::string& sep,
                  bool omit_empty = false) {
  return split_cb([out](const std::string& val) {
    sfu::container_traits<Container>::add_element(*out, val);
    return true;
  }, str, sep, omit_empty);
}

template<class I = InputIterator> inline
size_t join_into(std::string* out,
                 I begin,
                 I end,
                 const std::string& sep) {
  size_t i = 0;
  for (;begin != end; ++begin) {
    if (!out->empty()) {
      out->append(sep);
    }
    out->append(*begin);
    ++i;
  }
  return i;
}

template<class I = InputIterator> inline
const std::string join(I begin,
                       I end,
                       const std::string& sep) {
  std::string out;
  join_into(&out, begin, end, sep);
  return out;
}

template<class Container> inline
const std::string join(const Container& c, const std::string& sep) {
  std::string out;
  join_into(&out, c.begin(), c.end(), sep);
  return out;
}

size_t split_kvp_cb(std::function<bool(const std::string&,
                                       const std::string&)> out,
                    const std::string& str,
                    const std::string& pair_sep,
                    const std::string& kv_sep,
                    // If omit_empty_pairs is true, skips empty pairs,
                    // otherwise stops.
                    bool omit_empty_pairs = false,
                    // If omit_empty_values is true, will skip pairs with no
                    // value sep or no content after value sep.
                    bool omit_empty_values = false,
                    // If true allows kvp with no value separator, streating
                    // the whole pair as the key with empty value.
                    bool allow_no_sep = false);

// AssociativeContainer must have insert(pair<string,string>) method.
template<class AssociativeContainer> inline
size_t split_kvp_into(AssociativeContainer* out,
                      const std::string& str,
                      const std::string& pair_sep,
                      const std::string& kv_sep,
                      bool omit_empty_pairs = false,
                      bool omit_empty_values = false,
                      bool allow_no_sep = false) {
  return split_kvp_cb(
      [out](const std::string& key,
            const std::string& value) {
        // Supports std::map, std::multimap and std::unordered_map
        out->insert(std::pair<std::string,std::string>(key, value));
        return true;
      }, str, pair_sep, kv_sep,
      omit_empty_pairs, omit_empty_values, allow_no_sep);
}

std::map<std::string, std::string> split_kvp(
    const std::string& str,
    const std::string& pair_sep,
    const std::string& kv_sep,
    bool omit_empty_pairs = false,
    bool omit_empty_values = false,
    bool allow_no_sep = false);

// has_* functions check for certain properties of the string.
//
// bool has_prefix(string, string);
// bool has_suffix(string, string);

// strip_* removes a specific string from either the beginning or end of the
// target string. Returns whether the string was stripped.
//
// bool strip_prefix(*string, string);
// bool strip_suffix(*string, string);
//
// trim_* removes all continuous set of characters from either side (or both)
// that matches the function matcher. Returns number of characters trimmed.
//
// size_t trim(*string, bool(char));
// size_t trim_left(*string, bool(char));
// size_t trim_right(*string, bool(char));
//
// trim_whitespace removes all ASCII whitespace or blank characters from the
// beginning and end of the string. Is equivalent to:
//   trim(str, []bool(char c){ return isblank(c) || isspace(c); })
// but does not use callbacks for efficiency. Returns number of characters
// trimmed.
//
// size_t trim_whitespace(*string);

bool has_prefix(const std::string& str, const std::string& prefix);
bool has_suffix(const std::string& str, const std::string& suffix);

bool strip_prefix(std::string* str, const std::string& prefix);
bool strip_suffix(std::string* str, const std::string& suffix);

size_t trim(std::string* str, const std::function<bool(char)>& f);
size_t trim_left(std::string* str, const std::function<bool(char)>& f);
size_t trim_right(std::string* str, const std::function<bool(char)>& f);
size_t trim_whitespace(std::string* str);

}  // namespace strings
}  // namespace sfu

#endif  // SFU_STRINGS_H_
