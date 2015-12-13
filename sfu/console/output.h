#ifndef CONSOLE_OUTPUT_H_
#define CONSOLE_OUTPUT_H_

#include <functional>
#include <termios.h>

namespace sfu {
namespace console {

// Gets the current size in rows and columns of the terminal.
bool terminal_size(int* rows, int* cols);

}  // namespace console
}  // namespace sfu

#endif  // CONSOLE_OUTPUT_H_
