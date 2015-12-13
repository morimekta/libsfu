#ifndef CONSOLE_INPUT_H_
#define CONSOLE_INPUT_H_

#include <functional>
#include <termios.h>

#include "sfu/console/char.h"

namespace sfu {
namespace console {

// Get a keystroke represented as a char from the console.
const Char getkey();

// Keep the console in raw input mode (nice to combine with getkey). E.g.
// {
//   RawInput inp;
//   return getkey();
// }
class RawInput {
  public:
    RawInput();
    ~RawInput();

    // special method to suspend the raw mode until called method has returned.
    template<typename C>
    inline C suspend(std::function<C()> run) {
      end_raw();
      C out = run();
      set_raw();
      return out;
    }

  private:
    RawInput(const RawInput& i) {}
    void operator =(const RawInput& i) {}

    void set_raw();
    void end_raw();

    struct termios default_attrs_;
    struct termios raw_attrs_;
};

// Get a keystroke represented as a char from the console. Convenience method
// to combine RawInput with getkey.
const Char getchar();

}  // namespace console
}  // namespace sfu

#endif  // CONSOLE_INPUT_H_
