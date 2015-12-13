#include <stdio.h>
#include <termios.h>
#include <unistd.h>

#include <map>

#include "sfu/console/char.h"
#include "sfu/console/input.h"
#include "sfu/utf8.h"

namespace sfu {
namespace console {
namespace input_internal {

static std::map<Char,Char> translate;
void initTranslateMap() {
  if (translate.empty()) {
    translate[Char("\x1bOH")] = Char::HOME;
    translate[Char("\x1bOF")] = Char::END;
  }
}

const Char normalize(const Char& ch) {
  initTranslateMap();
  std::map<Char,Char>::iterator i = translate.find(ch);
  if (i != translate.end()) {
    return i->second;
  }
  return ch;
}

}  // namespace input_internal

const Char getkey() {
  int c = fgetc(stdin);
  if (c != (int) kESC && c <= (int) kDEL) {
    if (c == kDEL) return Char::BACKSPACE;
    return Char((char) c);
  } else if (c > (int) kDEL) {
    // Utf8 encoded unicode char.
    char buffer[6] = {(char) c, 0, };
    size_t len = sfu::Utf8LengthFromBuffer(buffer, 6);
    for (size_t i = 1; i < len; ++i) {
      buffer[i] = fgetc(stdin);
    }
    return Char(buffer, len);
  }

  char buffer[MAX_CHAR_LENGTH] = {(char) c, 0, };
  for (size_t i = 1; i < MAX_CHAR_LENGTH; ++i) {
    buffer[i] = fgetc(stdin);
    if (buffer[i] == kESC) {
      return Char::ESC;
    }
    Char tmp(buffer, i + 1);
    if (tmp.is_escape(true)) {
      return input_internal::normalize(tmp);
    }
  }

  return Char::EOT;
}

RawInput::RawInput() {
  tcgetattr(STDIN_FILENO, &default_attrs_);
  raw_attrs_ = default_attrs_;
  cfmakeraw(&raw_attrs_);
  set_raw();
}

RawInput::~RawInput() {
  end_raw();
}

void RawInput::set_raw() {
  tcsetattr(STDIN_FILENO, TCSANOW, &raw_attrs_);
}

void RawInput::end_raw() {
  tcsetattr(STDIN_FILENO, TCSANOW, &default_attrs_);
}

const Char getchar() {
  RawInput input;
  return getkey();
}

}  // namespace console
}  // namespace sfu
