#ifndef CONSOLE_INTERACTIVE_H_
#define CONSOLE_INTERACTIVE_H_

#include <functional>
#include <stdio.h>
#include <termios.h>

#include "sfu/console/char.h"

namespace sfu {
namespace console {

// Get a console confirmation [Yes / No].
bool confirm(const std::string& message, bool* result);

class LineReader {
  public:
    LineReader(const std::string& message,
        std::function<bool(const Char& c, std::string* msg)> char_verifier,
        std::function<bool(const std::string&, std::string* msg)> result_verifier);
    ~LineReader();

    bool read(std::string* out);

  private:
    bool handle_delete_(const Char& c);
    bool handle_direction_(const Char& c);
    bool handle_tab_(const Char& c);

    void print_warning_(const std::string& warning);


    std::string message_;
    size_t message_length_;

    std::string before_cursor_;
    std::string after_cursor_;
    int after_length_;
    Char last_char_;
    bool printed_warning_;

    std::function<bool(const Char& c, std::string* msg)> char_verifier_;
    std::function<bool(const std::string&, std::string* msg)>
      result_verifier_;

    // tab_expander_
    // tab_alternatives_
};

}  // namespace console
}  // namespace sfu

#endif  // CONSOLE_INTERACTIVE_H_
