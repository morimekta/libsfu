#include <sys/ioctl.h>
#include <stdio.h>

#include "sfu/console/output.h"

namespace sfu {
namespace console {

bool terminal_size(int* rows, int* cols) {
  struct winsize w;
  if (ioctl(0, TIOCGWINSZ, &w) != 0) {
    return false;
  }

  *rows = w.ws_row;
  *cols = w.ws_col;
  return true;
};

}  // namespace console
}  // namespace sfu
