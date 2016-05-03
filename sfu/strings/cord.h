#ifndef SFU_STRINGS_CORD_H_
#define SFU_STRINGS_CORD_H_

#include <string>
#include <utility>
#include <iostream>

namespace sfu {
namespace strings {

class cord {
 private:
  const char* ptr_;
  size_t      len_;

 public:
  static const size_t npos;

  typedef const char* iterator;
  typedef const char* const_iterator;

  cord();
  // Allow default copy constructor. And also allow single argument
  // constructors without being explicit.
  // cord(const cord& strp);
  cord(const cord& strp, size_t len);
  cord(const char* ptr);
  cord(const char* ptr, size_t len);
  cord(const std::string& str);
  cord(const std::string& str, size_t len);

  inline const char* ptr() const { return ptr_; }
  inline size_t length() const { return len_; };

  // Support basic itertors.
  inline const char* begin() const { return ptr_; }
  inline const char* end() const { return ptr_ + len_; }

  size_t find(const cord& str) const;
  // size_t find_last(const cord& str) const;
  size_t find(char c) const;
  size_t find_last(char c) const;

  inline void reset(const char* ptr, size_t len) {
    // assert(len >= 0);
    ptr_ = ptr;
    len_ = len;
  }

  inline void reset(const cord& sp) {
    ptr_ = sp.ptr();
    len_ = sp.length();
  }

  void reset(const char* ptr);
  void reset(const std::string& str);

  inline const char operator[](size_t pos) const {
    // assert(pos < len_);
    return *(ptr_ + pos);
  }
  inline const char at(size_t pos) const {
    // assert(pos < len_);
    return *(ptr_ + pos);
  }

  // Compare the string contents of the cord.
  bool string_equals(const cord& other) const;
  bool string_less_than(const cord& other) const;

  const std::string as_string() const;
};

}  // namespace strings
}  // namespace sfu

namespace std {
  // NOTE: We use the std::less specialization, NOT making a generic
  // operator<() implementation. This is becuase this cords could as
  // easily be compared to where they pointed to in memory instead.  This is to
  // be able to use the cord as a sorted key to map<> on an equal ground
  // with strings.
  template<> struct less<sfu::strings::cord> {
    bool operator()(const sfu::strings::cord& lhs,
                    const sfu::strings::cord& rhs) const {
      return lhs.string_less_than(rhs);
    }
  };
}

std::ostream& operator<<(std::ostream& out,
                         const sfu::strings::cord& sp);

#endif // SFU_STRINGS_CORD_H_
