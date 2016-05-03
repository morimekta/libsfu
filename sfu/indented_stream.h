#ifndef SFU_INDENTED_STREAM_H_
#define SFU_INDENTED_STREAM_H_

#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>

#include "sfu/console/char.h"

namespace sfu {

class IndentedStream : public std::basic_ostream<char> {
  public:
    IndentedStream(std::ostream* out);
    ~IndentedStream();

    IndentedStream& begin();
    IndentedStream& begin(const std::string& indent);
    IndentedStream& end();

    IndentedStream& put(char c);
    IndentedStream& put(const console::Char& c);
    IndentedStream& write(const std::string& str);
    IndentedStream& write(const char* str);
    IndentedStream& write(const char* str, size_t n);
    IndentedStream& newline();

    inline sfu::IndentedStream& operator<<(char c) {
      return put(c);
    }

    inline sfu::IndentedStream& operator<<(const sfu::console::Char& c) {
      return put(c);
    }

    inline sfu::IndentedStream& operator<<(const std::string& str) {
      return write(str);
    }

    inline sfu::IndentedStream& operator<<(const char* const str) {
      return write(str);
    }


  private:
    void maybe_indent();
   
    std::string indent_;
    std::vector<std::string> stack_;

    // Set to true after each newline. Triggers indent insersion on write.
    bool clear_line_;
    // Target output stream.
    std::ostream* out_;
};

}  // namespace sfu

inline sfu::IndentedStream&
operator<<(sfu::IndentedStream& is, std::ostream& (*pf)(std::ostream&)) {
  (*pf)(is);
  return is;
}

inline sfu::IndentedStream&
operator<<(sfu::IndentedStream& is, std::ios& (*pf)(std::ios&)) {
  (*pf)(is);
  return is;
}

inline sfu::IndentedStream&
operator<<(sfu::IndentedStream& is, std::ios_base& (*pf)(std::ios_base&)) {
  (*pf)(is);
  return is;
}

#endif
