#include <cstring>
#include <cstdarg>

#include "sfu/console/char.h"
#include "sfu/strings/format.h"
#include "sfu/utf8.h"

#include "sfu/encoding.h"

namespace sfu {
namespace console {

Char::Char() : len_(0) {
  buffer_[0] = '\0';
}
Char::Char(int32_t cp) : len_(sfu::Utf8LengthFromCodepoint(cp)) {
  sfu::Utf8FromCodepoint(cp, buffer_, len_);
}
Char::Char(char c) : len_(1) {
  buffer_[0] = c;
  buffer_[1] = '\0';
}
Char::Char(const Char& c) : len_(c.len_) {
  memcpy(buffer_, c.buffer_, len_);
  buffer_[len_] = '\0';
}
Char::Char(const sfu::strings::cord& str) : Char(str.ptr(), str.length()) {}
Char::Char(const char* str) : Char(str, strlen(str)) {}
Char::Char(const char* str, size_t len) : len_(std::min(len, MAX_CHAR_LENGTH)) {
  memcpy(buffer_, str, len_);
  buffer_[len_] = '\0';
}

Char& Char::operator=(const Char& c) {
  memcpy(buffer_, c.buffer_, c.len_ + 1);
  len_ = c.len_;
  return *this;
}

int Char::display_width() const {
  if (is_escape() || is_control()) return 0;
  if (len_ == 1) return 1;
  if (is_unicode()) {
    int32_t cp = codepoint();
    // Main block of CJK chars.
    if (cp >= 0x4e00 && cp < 0xa000) return 2;
  }
  return 1;
}

int32_t Char::hash() const {
  int64_t val = 479001599;
  for (size_t i = 0; i < len_; ++i) {
    val += buffer_[i];
    val *= 719;
  }
  return val % 87178291199;
}

int32_t Char::codepoint() const {
  if (!is_unicode()) {
    return 0;
  }
  if (len_ == 1) return buffer_[0];
  return sfu::Utf8ToCodepoint(buffer_, len_);
}

int32_t Char::ord() const {
  if (len_ == 1) {
    return buffer_[0];
  }
  if (is_unicode()) {
    return codepoint();
  }
  return -(hash() % 0x80000000);
}

int Char::compare(const Char& c) const {
  bool is_c = is_escape();
  bool other_c = c.is_escape();
  if (is_c && other_c) {
    int cmp = strncmp(c_str(), c.c_str(), std::min(length(), c.length()));
    if (cmp == 0) return c.length() - len_;
    return cmp;
  } else if (is_c) {
    return -1;
  } else if (other_c) {
    return 1;
  }
  return c.ord() - ord();
}

// Is a printable ASCII char.
bool Char::is_ascii() const {
  return len_ == 1 && buffer_[0] >= ' ' && buffer_[0] < kDEL;
};

// Is a control char or control sequence.
bool Char::is_control(bool valid) const {
  if (len_ == 0) return false;
  if (len_ == 1 && buffer_[0] >= 0 &&
      (buffer_[0] < ' ' || buffer_[0] == kDEL)) return true;
  if (buffer_[0] < 0) {  // Cheap 
    int32_t o = sfu::Utf8ToCodepoint(buffer_, len_);
    return o > 0x80 && o < 0xa0;
  }

  return false;
}

bool Char::is_escape(bool valid) const {
  if (len_ == 0) return false;
  if (buffer_[0] != kESC || len_ == 1) return false;
  if (valid) {
    // Beginning with [, 
    if (buffer_[1] == '[') {
      if (len_ == 3) {
        // ESC + [A-Z]
        return buffer_[2] >= 'A' && buffer_[2] <= 'Z';
      }
      bool last_was_num = false;
      bool has_sep = false;
      for (size_t p = 2; p < len_ - 1; ++p) {
        if (buffer_[p] == ';' && last_was_num) {
          has_sep = true;
          last_was_num = false;
          continue;
        }
        if (buffer_[p] >= '0' && buffer_[p] <= '9') {
          last_was_num = true;
          continue;
        }
        // The char is neither a numver not a separator.
        return false;
      }
      // ESC + [ + number + '~', e.g. F5.
      // ESC + [ + number + (;number)* + [mABCD]
      //  - m = color scheme
      //  - A-D = numbered cursor movement, or alt/ctrl cursor movement.
      return last_was_num && (
          (!has_sep && buffer_[len_ - 1] == '~') ||
          buffer_[len_ - 1] == 'm' ||
          (buffer_[len_ - 1] >= 'A' && buffer_[len_ - 1] <= 'D'));

    } else if (buffer_[1] == 'O') {
      return len_ == 3 && buffer_[2] >= 'A' && buffer_[2] <= 'Z';
    }
    // ESC + [a-zA-NP-Z].
    return len_ == 2 && isalpha(buffer_[1]);
  }
  return true;
}

bool Char::is_color(bool valid) const {
  if (length() < 3 || buffer_[0] != kESC || buffer_[1] != '[' ||
      buffer_[length() - 1] != 'm') {
    return false;
  }

  if (!valid) return true;

  bool last_was_num = false;
  size_t len = length() - 1;
  for (size_t p = 2; p < len; ++p) {
    if (buffer_[p] == ';' && last_was_num) {
      last_was_num = false;
      continue;
    }
    if (buffer_[p] >= '0' && buffer_[p] <= '9') {
      last_was_num = true;
      continue;
    }
    return false;
  }

  return true;
}

bool Char::is_unicode(bool valid) const {
  if (len_ == 0) {
    return false;
  } else if (buffer_[0] >= 0 && (buffer_[0] < ' ' || buffer_[0] == kDEL)) {
    // Escape and control sequences are not unicode.
    return false;
  } else if (len_ == 1) {
    // All  other ASCII single chars are valid UTF-8 (and unicode).
    return buffer_[0] > 0 && buffer_[0] < kDEL;
  }

  if (valid) {
    size_t len = sfu::Utf8LengthFromBuffer(buffer_, len_);
    if (len != len_) {
      return false;
    }
    int32_t cp = sfu::Utf8ToCodepoint(buffer_, len_);
    return cp > 0;
  }

  return true;
}

bool Char::is_valid() const {
  return len_ > 0 && (is_unicode(true) || is_control(true) ||
                      is_escape(true) || is_color(true));
}

const std::string Char::as_string(bool printable) const {
  return std::string(buffer_, len_);
}

const Char Char::mkcolor(uint8_t col, ...) {
  char buffer[MAX_CHAR_LENGTH + 1] = {kESC, '[', 0, };

  size_t p = 2;

  uint8_t num = col;
  size_t s = snprintf(buffer + p, MAX_CHAR_LENGTH - 1,
                      "%i", num);
  if (s < 0) return Char();
  p += s;
  if (num) {
    va_list pa;
    va_start(pa, col);

    while (p < MAX_CHAR_LENGTH - 4) {
      num = va_arg(pa, int);
      if (!num) break;

      s = snprintf(buffer + p, MAX_CHAR_LENGTH - 1,
                   ";%i", num);
      if (s < 0) break;
      p += s;
    }

    va_end(pa);
  }

  buffer[p++] = 'm';
  return Char(buffer, p);
}

const Char Char::cursor_setpos(int line, int column) {
  return Char(sfu::strings::format("\x1b[%d;%dH", line, column));
}
const Char Char::cursor_up(int num) {
  if (num == 1) return Char::UP;
  return Char(sfu::strings::format("\x1b[%iA", num));
}
const Char Char::cursor_down(int num) {
  if (num == 1) return Char::DOWN;
  return Char(sfu::strings::format("\x1b[%iB", num));
}
const Char Char::cursor_right(int num) {
  if (num == 1) return Char::RIGHT;
  return Char(sfu::strings::format("\x1b[%iD", num));
}
const Char Char::cursor_left(int num) {
  if (num == 1) return Char::LEFT;
  return Char(sfu::strings::format("\x1b[%iD", num));
}

const Char Char::numeric(int num) {
  if (num > 0) {
    if (num < 21) {
      // 0x2460 - 0x2473 for 1 - 20
      return Char(0x2460 -  1 + num);
    }
    if (num < 36) {
      return Char(0x3250 - 20 + num);
    }
    if (num < 51) {
      return Char(0x32b0 - 35 + num);
    }
  }
  return Char();
}

bool Char::FromString(const char* str, size_t maxlen, Char* ch) {
  if (maxlen == 0) {
    return false;
  } else if (str[0] == kESC) {
    // Escape Sequence.
    for (size_t len = 2; len <= maxlen; ++len) {
      Char tmp(str, len);
      if (tmp.is_escape(true)) {
        *ch = tmp;
        return true;
      }
    }
  } else if (str[0] < 0) {
    // Utf8 encoded char.
    size_t len = sfu::Utf8LengthFromBuffer(str, maxlen);
    if (len > 0) {
      *ch = Char(str, len);
      return true;
    }
  } else {
    *ch = Char(str, 1);
    return true;
  }
  return false;
}

bool Char::LastFromString(const char* str, size_t maxlen, Char* ch) {
  if (maxlen == 0) {
    return false;
  }

  // Otherwise iterate over string to find the last character.
  // TODO(steineldar): Make faster version for printable strings only (where
  // the char would either be ASCII (1 byte always) or UTF-8, which can be
  // back-tracked).
  size_t pos = 0;
  Char last;
  while (pos < maxlen) {
    if (!FromString(str + pos, maxlen - pos, &last)) {
      return false;
    }
    pos += last.length();
  }

  *ch = last;
  return true;
}

int Char::DisplayWidth(const std::string& str) {
  size_t pos = 0;
  int width = 0;
  Char c;
  while (pos < str.size()) {
    if (!FromString(str.c_str() + pos, str.size() - pos, &c)) {
      // ERROR.
      return width;
    }

    width += c.display_width();
    pos += c.length();
  }

  return width;
}

// Strip away all control and escape characters, and leave the rest.
bool Char::DisplayString(const std::string& str, std::string* out) {
  size_t pos = 0;
  Char c;
  while (pos < str.size()) {
    if (!FromString(str.c_str() + pos, str.size() - pos, &c)) {
      return false;
    }
    if (c.display_width() > 0) {
      out->append(c.c_str(), c.length());
    }
    pos += std::max((size_t) 1, c.length());
  }
  return true;
}

std::ostream& operator<<(std::ostream& out, const Char& c) {
  out << c.c_str();
  return out;
}

// ------------------------------
// --  Characters to remember  --
// ------------------------------

// Clear all colors to default.
const uint8_t Char::Color::CLEAR      =  0;

// Foreground (text) color.
const uint8_t Char::Color::DEFAULT    = 39;
const uint8_t Char::Color::BLACK      = 30;
const uint8_t Char::Color::RED        = 31;
const uint8_t Char::Color::GREEN      = 32;
const uint8_t Char::Color::YELLOW     = 33;
const uint8_t Char::Color::BLUE       = 34;
const uint8_t Char::Color::MAGENTA    = 35;
const uint8_t Char::Color::CYAN       = 36;
const uint8_t Char::Color::WHITE      = 37;

// Bacground colors.
const uint8_t Char::Color::BG_DEFAULT = 49;
const uint8_t Char::Color::BG_BLACK   = 40;
const uint8_t Char::Color::BG_RED     = 41;
const uint8_t Char::Color::BG_GREEN   = 42;
const uint8_t Char::Color::BG_YELLOW  = 43;
const uint8_t Char::Color::BG_BLUE    = 44;
const uint8_t Char::Color::BG_MAGENTA = 45;
const uint8_t Char::Color::BG_CYAN    = 46;
const uint8_t Char::Color::BG_WHITE   = 47;

// Base modifiers
const uint8_t Char::Color::BOLD       =  1;
const uint8_t Char::Color::DIM        =  2;
const uint8_t Char::Color::UNDERLINE  =  4;
const uint8_t Char::Color::STROKE     =  9;
const uint8_t Char::Color::INVERT     =  7;
const uint8_t Char::Color::HIDDEN     =  8;

// Humm...
// BOLD (bright) = 1
// UNDERLINE = 2, 4
// INVERT = 3, 7
// BRIGHT_BG = 5
// 

// NOT_ values unset the matching modifier.
const uint8_t Char::Color::NOT_BOLD      = 21;
const uint8_t Char::Color::NOT_DIM       = 22;
const uint8_t Char::Color::NOT_UNDERLINE = 24;
const uint8_t Char::Color::NOT_STROKE    = 29;
const uint8_t Char::Color::NOT_INVERT    = 27;
const uint8_t Char::Color::NOT_HIDDEN    = 28;


const Char Char::ESC       ('\x1b');  // \e
const Char Char::DEL       ('\x7f');

const Char Char::BACKSPACE ('\x08');
const Char Char::ABORT     ('\x03');  // ^C End of Text.
const Char Char::EOT       ('\x04');  // ^D End of Transmission (EOF).
const Char Char::CANCEL    ('\x18');  // ^X Cancel

const Char Char::BELL      ('\x07');  // \a
const Char Char::NEWLINE   ('\x0a');  // \n LF
const Char Char::RETURN    ('\x0d');  // \r CR
const Char Char::TAB       ('\x09');  // \t
// LINE_TAB \x0b

// CANCEL \x18

const Char Char::UP        ("\x1b[A");
const Char Char::DOWN      ("\x1b[B");
const Char Char::RIGHT     ("\x1b[C");
const Char Char::LEFT      ("\x1b[D");

const Char Char::CURSOR_ERASE  ("\x1b[K");
const Char Char::CURSOR_SAVE   ("\x1b[s");
const Char Char::CURSOR_RESTORE("\x1b[u");

const Char Char::CTRL_UP   ("\x1b[1;5A");
const Char Char::CTRL_DOWN ("\x1b[1;5B");
const Char Char::CTRL_RIGHT("\x1b[1;5C");
const Char Char::CTRL_LEFT ("\x1b[1;5D");

const Char Char::DPAD_MID  ("\x1b[E");

const Char Char::INSERT    ("\x1b[2~");
const Char Char::DELETE    ("\x1b[3~");
const Char Char::HOME      ("\x1b[1~");  // alt: \x1bOH
const Char Char::END       ("\x1b[4~");  // alt: \x1bOF
const Char Char::PAGE_UP   ("\x1b[5~");
const Char Char::PAGE_DOWN ("\x1b[6~");

const Char Char::F1        ("\x1bOP");
const Char Char::F2        ("\x1bOQ");
const Char Char::F3        ("\x1bOR");
const Char Char::F4        ("\x1bOS");
const Char Char::F5        ("\x1b[15~");
const Char Char::F6        ("\x1b[17~");
const Char Char::F7        ("\x1b[18~");
const Char Char::F8        ("\x1b[19~");
const Char Char::F9        ("\x1b[20~");

const Char Char::APOS      ('\'');
const Char Char::QUOTE     ('"');

const Char Char::LSQUO     ("\u2018");
const Char Char::RSQUO     ("\u2019");
const Char Char::BSQUO     ("\u201a");
const Char Char::LDQUO     ("\u201c");
const Char Char::RDQUO     ("\u201d");
const Char Char::BDQUO     ("\u201e");

const Char Char::LSAQUO    ("\u2039");
const Char Char::RSAQUO    ("\u203a");
const Char Char::LAQUO     ("\u00ab");
const Char Char::RAQUO     ("\u00bb");

const Char Char::DOLLAR    ('$');
const Char Char::CENT      ("\u00a2");
const Char Char::POUND     ("\u00a3");
const Char Char::YEN       ("\u00a5");
const Char Char::EURO      ("\u20ac");

const Char Char::SECTION   ("\xc2\xa7");
const Char Char::COPYRIGHT ("\xc2\xa9");
const Char Char::REGISTERED("\xc2\xae");
const Char Char::TRADEMARK ("\xe2\x84\xa2");

const Char Char::DEGREES   ("\xc2\xb0");
const Char Char::MDOT      ("\xc2\xb7");

const Char Char::FRACT_1Q  ("\xc2\xbc");
const Char Char::FRACT_1H  ("\xc2\xbd");
const Char Char::FRACT_3Q  ("\xc2\xbe");

const Char Char::INFINITE  ("\u221e");

}  // namespace console
}  // namespace sfu
