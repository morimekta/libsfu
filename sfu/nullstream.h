#ifndef UTIL_NULLSTREAM_H_
#define UTIL_NULLSTREAM_H_

#include <iostream>

namespace sfu {

class NullStream : public std::ostream {
 public:
  NullStream();
  ~NullStream();
};

// Not out stream...
static NullStream nout;

}  // namespace sfu

template<typename T> inline sfu::NullStream&
operator<<(sfu::NullStream& s, const T& t) { return s; }

inline sfu::NullStream&
operator<<(sfu::NullStream& s, std::ostream &(std::ostream&)) { return s; }

#endif
