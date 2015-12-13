#include <map>
#include <iostream>

#include "sfu/console/input.h"
#include "sfu/console/interactive.h"
#include "sfu/encoding.h"
#include "sfu/utf8.h"

namespace sfu {
namespace console {

bool confirm(const std::string& message, bool* result) {
  // TODO(steineldar): 
  //  if alternative output mode: print('\n')
  std::cout << "\r" << Char::CURSOR_ERASE << message << " [Y/n]: "
            << std::flush;

  // console::increment_linecounter()

  RawInput input;

  while (true) {
    Char c = getkey();

    if (c == 'Y' || c == 'y' || c == Char::RETURN) {
      std::cout << "\r\n" << std::flush;
      *result = true;
      return true;
    } else if (c == 'N' || c == 'n' || c == Char::BACKSPACE) {
      std::cout << "\r\n" << std::flush;
      *result = false;
      return true;
    } else if (c == Char::ESC || c == Char::ABORT || c == Char::EOT ||
               c == Char::CANCEL) {
      std::cout << "\r\n" << std::flush;
      return false;
    } else if (c == ' ' || c == '\t') {
      continue;
    }

    std::cout << "\r" << Char::CURSOR_ERASE << message << " [Y/n]: "
              // TODO(steineldar): Replace CEscape with printable-version.
              // E.g. "<up>" or "<F4>" instead of "\x7f[A" or "\x7fOP".
              << "'" << sfu::CEscape(c.c_str())
              << "' is not valid input, select 'y' or 'n'."
              << std::flush;
  }

  return false;
}


bool accept_all_chars(const Char& c, std::string *msg) {
  if (c.is_ascii() || c.is_unicode()) {
    return true;
  }

  // Non-printable character.
  *msg = "Non printable character.";
  return false;
}

bool accept_all_results(const std::string& c, std::string *msg) {
  return true;
}


LineReader::LineReader(const std::string& message,
    std::function<bool(const Char& c, std::string* msg)> char_verifier,
    std::function<bool(const std::string&, std::string* msg)> result_verifier) :
message_(message), char_verifier_(char_verifier),
result_verifier_(result_verifier) {
  if (char_verifier_ == NULL) {
    char_verifier_ = std::bind(accept_all_chars,
        std::placeholders::_1, std::placeholders::_2);
  }
  if (result_verifier_ == NULL) {
    result_verifier_ = std::bind(accept_all_results,
        std::placeholders::_1, std::placeholders::_2);
  }
  message_length_ = Char::DisplayWidth(message_);
}

LineReader::~LineReader() {}

// Content of line at call time becomes the initial input.
bool LineReader::read(std::string* line) {
  before_cursor_ = "";
  if (!Char::DisplayString(*line, &before_cursor_)) {
    *line = "";
  }

  after_cursor_ = "";
  after_length_ = 0;
  last_char_ = Char();
  printed_warning_ = false;

  std::string warning;
  RawInput input;
  while (true) {
    std::cout << "\r" << Char::CURSOR_ERASE << message_
              << " " << before_cursor_;
    if (after_length_ > 0) {
      std::cout << after_cursor_ << Char::cursor_left(after_length_);
    }
    std::cout << std::flush;

    Char c = getkey();
    if (c == Char::RETURN) {
      std::string result = before_cursor_ + after_cursor_;
      if (result_verifier_ == NULL) {
        if (!result_verifier_(result, &warning)) {
          print_warning_(warning);
          last_char_ = c;
          continue;
        }
      }
      std::cout << "\r\n" << std::flush;
      *line = result;
      return true;
    } else if (c == Char::ESC || c == Char::ABORT || c == Char::EOT ||
               c == Char::CANCEL) {
      std::cout << "\r\n" << std::flush;
      return false;
    } else if (handle_delete_(c)) {
    } else if (handle_direction_(c)) {
    } else if (handle_tab_(c)) {
    } else if (char_verifier_(c, &warning)) {
      // Just print this and whatever is after.
      before_cursor_.append(c.c_str(), c.length());
      std::cout << c;
      if (after_length_ > 0) {
        std::cout << after_cursor_ << Char::cursor_left(after_length_);
      }
      std::cout << std::flush;
    } else if (warning.size() > 0) {
      // TODO(steineldar): Add ""
      print_warning_(warning);
      warning = "";
    }

    last_char_ = c;
  }

  return false;
}

// ------------------ PRIVATE ----------------
bool LineReader::handle_delete_(const Char& c) {
  Char tmp;
  if (c == Char::DELETE) {
    // Delete first char after cursor.
    if (Char::FromString(after_cursor_.c_str(), after_cursor_.size(), &tmp)) {
      after_cursor_.erase(after_cursor_.begin(),
                          after_cursor_.begin() + tmp.length());
      after_length_ = Char::DisplayWidth(after_cursor_);
    }
    return true;
  } else if (c == Char::BACKSPACE) {
    // Delete last char before cursor.
    if (Char::LastFromString(before_cursor_.c_str(), before_cursor_.size(),
                             &tmp)) {
      before_cursor_.erase(before_cursor_.end() - tmp.length(),
                           before_cursor_.end());
    }
    return true;
  }
  return false;
}

bool LineReader::handle_direction_(const Char& c) {
  if (c == Char::HOME) {
    after_cursor_.insert(after_cursor_.begin(),
        before_cursor_.begin(), before_cursor_.end());
    after_length_ = Char::DisplayWidth(after_cursor_);
    before_cursor_.clear();
    return true;
  } else if (c == Char::END) {
    before_cursor_.insert(before_cursor_.end(),
        after_cursor_.begin(), after_cursor_.end());
    after_cursor_.clear();
    after_length_ = 0;
    return true;
  } else if (c == Char::LEFT) {
    Char tmp;
    if (Char::LastFromString(before_cursor_.c_str(), before_cursor_.size(),
                             &tmp)) {
      before_cursor_.erase(before_cursor_.end() - tmp.length(),
                           before_cursor_.end());
      std::string str(tmp.as_string());
      after_cursor_.insert(after_cursor_.begin(), str.begin(), str.end());
      after_length_ = Char::DisplayWidth(after_cursor_);
    }
    return true;
  } else if (c == Char::RIGHT) {
    Char tmp;
    // Get the first char after the cursor, and move it to before.
    if (Char::FromString(after_cursor_.c_str(), after_cursor_.size(), &tmp)) {
      after_cursor_.erase(after_cursor_.begin(),
                          after_cursor_.begin() + tmp.length());
      after_length_ = Char::DisplayWidth(after_cursor_);
      before_cursor_.append(tmp.c_str(), tmp.length());
    }
    return true;
  } else if (c == Char::CTRL_LEFT) {
    // Search for the next space ro the left, or go to the beginning.
    return true;
  } else if (c == Char::CTRL_RIGHT) {
    // Search for the next space ro the right, or go to the end.
    return true;
  } else if (c == Char::CTRL_UP || c == Char::CTRL_DOWN || c == Char::UP ||
             c == Char::DOWN) {
    return true;  // But ignore.
  }
  return false;
}

bool LineReader::handle_tab_(const Char& c) {
  return false;
}

void LineReader::print_warning_(const std::string& warning) {
  if (printed_warning_) {
    std::cout << Char::UP;
  }
  printed_warning_ = true;
  std::cout << "\r" << Char::CURSOR_ERASE
            << warning << "\r\n" << std::flush;
}

}  // namespace console
}  // namespace sfu
