#include <ctime>

namespace sfu {
namespace time {

time_t now() {
  return mktime(NULL);
}

};  // namespace time
};  // namespace sfu
