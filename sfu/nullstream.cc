#include "sfu/nullstream.h"

namespace sfu {

NullStream::NullStream() : std::ios(0), std::ostream(0) {}
NullStream::~NullStream() {}

}  // namespace sfu
