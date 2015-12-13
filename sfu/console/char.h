#ifndef CONSOLE_CHAR_H_
#define CONSOLE_CHAR_H_

#include <string>
#include <ostream>

#include "sfu/strings/cord.h"

namespace sfu {
namespace console {

// Make sure that we have space for a Char object on 32 bytes. This will block
// some of the more ambitious color control sequences, but that should be a
// severe corner case, since: INVERT and HIDDEN are much of the same, and BOLD
// and DIM are mutually exclusive (both modifies the color). And if more than a
// couple of color modifiers are used, it should rather be cleared than "unset
// X".
static const size_t MAX_CHAR_LENGTH = 32 - 1 - sizeof(size_t);

// Handy character symbol constants.
static const char kESC = '\x1b';
static const char kDEL = '\x7f';

// The 'Char' class is not just a single character, it is more of a wrapper for 
class Char {
  public:
    class Color {
      public:
        // Clear all colors to default.
        static const uint8_t CLEAR;

        // Foreground (text) color.
        static const uint8_t DEFAULT;
        static const uint8_t BLACK;
        static const uint8_t RED;
        static const uint8_t GREEN;
        static const uint8_t YELLOW;
        static const uint8_t BLUE;
        static const uint8_t MAGENTA;
        static const uint8_t CYAN;
        static const uint8_t WHITE;

        // Bacground colors.
        static const uint8_t BG_DEFAULT;
        static const uint8_t BG_BLACK;
        static const uint8_t BG_RED;
        static const uint8_t BG_GREEN;
        static const uint8_t BG_YELLOW;
        static const uint8_t BG_BLUE;
        static const uint8_t BG_MAGENTA;
        static const uint8_t BG_CYAN;
        static const uint8_t BG_WHITE;

        // Base modifiers
        static const uint8_t BOLD;
        static const uint8_t DIM;
        static const uint8_t UNDERLINE;
        static const uint8_t STROKE;
        static const uint8_t INVERT;
        static const uint8_t HIDDEN;

        // NOT_ values unset the matching modifier.
        static const uint8_t NOT_BOLD;
        static const uint8_t NOT_DIM;
        static const uint8_t NOT_UNDERLINE;
        static const uint8_t NOT_STROKE;
        static const uint8_t NOT_INVERT;
        static const uint8_t NOT_HIDDEN;

      private:
        // ...
        Color() {}
        Color(const Color& c) {}
    };

    static const Char ESC;
    static const Char DEL;

    static const Char BACKSPACE;  // Remove one char before cursor.
    static const Char ABORT;
    static const Char EOT;        // EOF essentially.
    static const Char CANCEL;

    static const Char BELL;
    static const Char NEWLINE;
    static const Char RETURN;
    static const Char TAB;

    static const Char UP;
    static const Char DOWN;
    static const Char RIGHT;
    static const Char LEFT;

    static const Char CURSOR_ERASE;    // Erase text after the cursor on same line.
    static const Char CURSOR_SAVE;     // Save theposition of the cursor.
    static const Char CURSOR_RESTORE;  // Restore position of the cursor.

    static const Char CTRL_UP;
    static const Char CTRL_DOWN;
    static const Char CTRL_RIGHT;
    static const Char CTRL_LEFT;

    static const Char DPAD_MID;

    static const Char INSERT;
    static const Char DELETE;          // Remove one char after cursor.
    static const Char HOME;
    static const Char END;
    static const Char PAGE_UP;
    static const Char PAGE_DOWN;

    static const Char F1;
    static const Char F2;
    static const Char F3;
    static const Char F4;
    static const Char F5;
    static const Char F6;
    static const Char F7;
    static const Char F8;
    static const Char F9;
    // F10-F12 are used by the terminal, no not accessible.

    static const Char APOS;
    static const Char QUOTE;

    static const Char LSQUO;     // Left single quote.
    static const Char RSQUO;     // Right single quote.
    static const Char BSQUO;     // Single low-9 quote: ,Quote'.
    static const Char LDQUO;     // Left double quote.
    static const Char RDQUO;     // Right double quote.
    static const Char BDQUO;     // Double low-9 quote: ,,Quote''.

    static const Char LSAQUO;    // Left single angle quote: <
    static const Char RSAQUO;    // Right single angoe quote: >
    static const Char LAQUO;     // Left angle quote: <<
    static const Char RAQUO;     // Right angle quote: >>

    static const Char DOLLAR;
    static const Char CENT;
    static const Char POUND;
    static const Char YEN;
    static const Char EURO;

    static const Char SECTION;   // or paragraph marker.
    static const Char COPYRIGHT;
    static const Char REGISTERED;
    static const Char TRADEMARK;

    static const Char DEGREES;
    static const Char MDOT;

    static const Char FRACT_1Q;
    static const Char FRACT_1H;
    static const Char FRACT_3Q;

    static const Char INFINITE;

    Char();
    Char(int32_t cp);
    Char(char c);
    Char(const Char& c);
    Char(const char* str);
    Char(const char* str, size_t len);
    Char(const sfu::strings::cord& str);
    Char& operator=(const Char& c);

    // Number of bytes in the character / key setup.
    inline size_t length() const { return len_; }
    inline const char* c_str() const { return buffer_; }
    // Number of positions printed when printed to console. Note that cursor
    // movements and other unprintable characters have a display_length of 0,
    // even if they have side effects. Most have 1, wide chars (e.g. CJK) are
    // 2, and escape and controls will have 0.
    int display_width() const;
    int32_t codepoint() const;
    int32_t hash() const;
    int32_t ord() const;

    int compare(const Char& c) const;
    bool operator==(const Char& c) const { return compare(c) == 0; }
    bool operator< (const Char& c) const { return compare(c) <  0; }
    bool operator> (const Char& c) const { return compare(c) >  0; }
    bool operator<=(const Char& c) const { return compare(c) <= 0; }
    bool operator>=(const Char& c) const { return compare(c) >= 0; }

    // Special case for equality with char.
    bool operator==(char c) const {
      if (len_ != 1) {
        return false;
      }
      return buffer_[0] == c;
    }

    // Is a printable ASCII character.
    bool is_ascii() const;
    // Is this a utf8 byte sequence.
    bool is_unicode(bool valid = false) const;
    // Any control ch:aracter or escape control sequence.
    bool is_control(bool valid = false) const;
    // Is an escape base control sequence for controlling a terminal.
    bool is_escape(bool valid = false) const;
    // Is a color setting escape sequence.
    bool is_color(bool valid = false) const;
    // Is a valid character... Meaning anything, but valid.
    bool is_valid() const;

    // Char sequence as a std string.
    const std::string as_string(bool printable = false) const;

    // Make a color char from a set of color codes.
    static const Char mkcolor(uint8_t col, ...);
    static const Char cursor_setpos(int line, int column = 0);
    static const Char cursor_up(int num);
    static const Char cursor_down(int num);
    static const Char cursor_right(int num);
    static const Char cursor_left(int num);

    static const Char numeric(int num);

    static bool   FromString(const char* str, size_t maxlen, Char* ch);
    static bool   LastFromString(const char* str, size_t maxlen, Char* ch);
    static int    DisplayWidth(const std::string& str);
    static bool   DisplayString(const std::string& str, std::string* out);
  protected:
    size_t len_;
    char buffer_[MAX_CHAR_LENGTH + 1];
};  // class Char

std::ostream& operator<<(std::ostream& out, const Char& c);

}  // namespace console
}  // namespace sfu

#endif  // CONSOLE_CHAR_H_
