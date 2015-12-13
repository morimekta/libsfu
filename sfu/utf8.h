#ifndef UTIL_UTF8_H_
#define UTIL_UTF8_H_

#include <cstdint>
#include <cstring>

namespace sfu {

// Get the utf8 encoded length from the first char of the given buffer, with
// maximum length of len. If the buffer contains an encoded utf8 length longer
// than len, return 0.
size_t Utf8LengthFromBuffer(const char* buffer, size_t len);

// Get the length of the encoded character based on the unicode codepoint
// value. 
size_t Utf8LengthFromCodepoint(int32_t cp);

// Get the bitmask used to get the real value from the first byte of a utf8
// string based on it's length.
char Utf8ByteMaskFromLength(size_t len);

// Get the encoded length bits based on the length to encode.
char Utf8SizeBitsFromLength(size_t len);

// Get the Utf8 codepoint (unicode value) from a utf8 encoded buffer.
void Utf8FromCodepoint(int32_t cp, char* buffer, size_t len);

// Get the Utf8 codepoint (unicode value) from a utf8 encoded buffer.
int32_t Utf8ToCodepoint(const char* buffer, size_t len);

}  // namespace sfu

#endif
