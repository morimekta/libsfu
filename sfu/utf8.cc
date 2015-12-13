#include "sfu/utf8.h"

#include <iostream>

namespace sfu {

size_t Utf8LengthFromBuffer(const char* buffer, size_t len) {
  if (len == 0) return 0;
  // Check for most common first: ASCII compatible char.
  // 0b0xxxxxxx
  if ((buffer[0] & 0x80) == 0) return 1;
  // Content bytes.
  if ((buffer[0] & 0xc0) == 0xf0) return 0;
  // 0b110xxxxx
  if ((buffer[0] & 0xe0) == 0xc0) return len > 1 ? 2 : 0;
  // 0b1110xxxx
  if ((buffer[0] & 0xf0) == 0xe0) return len > 2 ? 3 : 0;
  // 0b11110xxx
  if ((buffer[0] & 0xf8) == 0xf0) return len > 3 ? 4 : 0;
  // 0b111110xx
  if ((buffer[0] & 0xfc) == 0xf8) return len > 4 ? 5 : 0;
  // 0b1111110x
  if ((buffer[0] & 0xfe) == 0xfc) return len > 5 ? 6 : 0;

  return 0;
}

size_t Utf8LengthFromCodepoint(int32_t cp) {
  if (cp <          0) return 0;
  if (cp <       0x80) return 1;  //  7-bit, ASCII
  if (cp <      0x800) return 2;  // 11-bit
  if (cp <    0x10000) return 3;  // 16-bit
  if (cp <   0x200000) return 4;  // 21-bit
  if (cp <  0x4000000) return 5;  // 26-bit
  return 6;                       // 31-bit
}

char Utf8ByteMaskFromLength(size_t len) {
  switch(len) {
    case 1: return 0xff;
    case 2: return 0x1f;
    case 3: return 0x0f;
    case 4: return 0x07;
    case 5: return 0x03;
    default:
      return 0x01;
  }
}

char Utf8SizeBitsFromLength(size_t len) {
  switch(len) {
    case 1: return 0x00;
    case 2: return 0xc0;
    case 3: return 0xe0;
    case 4: return 0xf0;
    case 5: return 0xf8;
    case 6: return 0xfc;
    default:
      return 0x00;
  }
}

void Utf8FromCodepoint(int32_t cp, char* buffer, size_t len) {
  buffer[len] = '\0';
  if (len == 0 || cp <= 0) return;
  for (size_t p = len - 1; p > 0; --p) {
    buffer[p] = 0x80 | (cp % 0x40);
    cp /= 0x40;
  }
  buffer[0] = 0xff & (Utf8SizeBitsFromLength(len) | cp);
}

int32_t Utf8ToCodepoint(const char* buffer, size_t len) {
  size_t ulen = Utf8LengthFromBuffer(buffer, len);
  if (ulen == 0) return -1;
  int32_t cp = buffer[0] & Utf8ByteMaskFromLength(ulen);
  for (size_t p = 1; p < ulen; ++p) {
    // Check for invalid data byte.
    if ((buffer[p] & 0xc0) != 0x80) {
      std::cout << "invbyte" << std::endl;
      return 0;
    }
    cp *= 0x40;
    cp += (buffer[p] & 0x3f);
  }

  return cp;
}

}  // namespace sfu
