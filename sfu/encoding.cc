#include "sfu/encoding.h"

#include <algorithm>
#include <cstdio>

#include "sfu/strings/cord.h"
#include "sfu/utf8.h"

using namespace std;

namespace sfu {
namespace {

const char *kBase64ReplaceTable =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

const char *kWebSafeBase64ReplaceTable =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

const char kBase64PadChar = '=';

const char kBase64Reverse[128] = {
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  // 0 .. 15
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  //   .. 31
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,  //   .. 47
  52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,  //   .. 63
  -1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,  //   .. 79
  15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,  //   .. 95
  -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,  //  .. 111
  41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1,  //  .. 127
};

const char kWebSafeBase64Reverse[128] = {
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  // 0 .. 15
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  //   .. 31
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1,  //   .. 47
  52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,  //   .. 63
  -1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,  //   .. 79
  15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, 63,  //   .. 95
  -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,  //  .. 111
  41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1,  //  .. 127
};


inline bool IsPrintable(char c) {
  return (c > 31 && c < 127);
}


inline void AppendBackslashOctal(char c, string* out) {
  char buffer[4] = { '\\', '0', '0', '0'};
  unsigned char ord = static_cast<unsigned char>(c);
  for (size_t pos = 3; pos > 0 && ord > 0; --pos) {
    buffer[pos] = '0' + (ord % 8);
    ord /= 8;
  }
  out->append(buffer, 4);
}

inline bool AppendEncodedUnicode(int32_t cp, string* out) {
  if (cp < 0 || cp > 0xFFFF) return false;
  char buffer[6] = { '\\', 'u', '0', '0', '0', '0'};
  for (size_t pos = 5; pos > 1 && cp > 0; --pos) {
    int32_t hex = cp % 16;
    cp /= 16;
    if (hex > 9) {
      buffer[pos] = 'a' - 10 + hex;
    } else {
      buffer[pos] = '0' + hex;
    }
  }
  out->append(buffer, 6);
  return true;
}


inline bool IsBase64Char(char c, bool web_safe) {
  return isalpha(c) || (web_safe ? c == '-' || c == '_' : c == '/' || c == '+');
}


inline void Base64EncodeBlock(const strings::cord& block,
                              const char* table,
                              string *out) {
  char encoded_block[5] = {0, };
  switch (block.length()) {
    case 3:
      encoded_block[3]  = (block[2] & 0x3f);
      encoded_block[2]  = (block[2] & 0xc0) >> 6;
    case 2:
      encoded_block[2] += (block[1] & 0x0f) << 2;
      encoded_block[1]  = (block[1] & 0xf0) >> 4;
    case 1:
      encoded_block[1] += (block[0] & 0x03) << 4;
      encoded_block[0]  = (block[0] & 0xfc) >> 2;
      break;
    default:
      // assert(false);  short never happen.
      return;
  }

  for (size_t i = 0; i < 4; ++i) {
    encoded_block[i] = *(table + encoded_block[i]);
  }

  out->append(encoded_block, block.length() + 1);
}


inline bool Base64DecodeBlock(const strings::cord& block,
                              const char* table,
                              string *out) {
  char buffer[4] = {0, };
  size_t size = block.length();
  switch (size) {
    case 4:
      if (block[3] < 32 || block[3] > 126) return false;
      buffer[3] = *(table + block[3]);
    case 3:
      if (block[2] < 32 || block[2] > 126) return false;
      buffer[2] = *(table + block[2]);
    case 2:
      if (block[1] < 32 || block[1] > 126 ||
          block[0] < 32 || block[0] > 126) return false;

      buffer[1] = *(table + block[1]);
      buffer[0] = *(table + block[0]);
      break;
    default:
      return false;
  }

  for (size_t i = 0; i < 4; ++i) {
    if (buffer[i] < 0) return false;
  }

  char decoded[4] = {0, };
  decoded[0] = ( buffer[0]         << 2) + ((buffer[1] & 0x30) >> 4);
  decoded[1] = ((buffer[1] & 0x0f) << 4) + ((buffer[2] & 0x3c) >> 2);
  decoded[2] = ((buffer[2] & 0x03) << 6) + buffer[3];

  out->append(decoded, size - 1);
  return true;
}


inline char HexToByte(char c, bool require_uppercase) {
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'A' && c <= 'F') return c - 'A' + 10;
  if (c >= 'a' && c <= 'f' &&
      !require_uppercase) return c - 'a' + 10;
  return -1;
}

inline bool IsUrlReservedChar(char c) {
  if (c == '!' || c == '*' || c == '\'' || c == '(' || c == ')' ||
      c == ';' || c == ':' || c == '@'  || c == '&' || c == '=' ||
      c == '+' || c == '$' || c == ','  || c == '/' || c == '?' ||
      c == '#' || c == '[' || c == ']'  || c == '\"') return true;
  return false;
}

inline void AppendHexEncodeInternal(const strings::cord &data, string *onto, char a) {
  // assert(a == 'a' || a == 'A');
  size_t from = onto->size();
  onto->resize(from + data.length() * 2, '0');
  for (size_t i = 0; i < data.length(); ++i) {
    char c1 = (data[i] & 0xf0) >> 4;
    char c2 = (data[i] & 0x0f);
    size_t pos = from + i*2;
    (*onto)[pos]     = c1 < 10 ? '0' + c1 : a - 10 + c1;
    (*onto)[pos + 1] = c2 < 10 ? '0' + c2 : a - 10 + c2;
  }
}


inline bool AppendHexDecodeInternal(const strings::cord &hex,
                                    string *str,
                                    bool require_uppercase) {
  const size_t from = str->size();
  if (hex.length() % 2 != 0) return false;
  const size_t add = hex.length() / 2;
  str->resize(from + add);
  for (size_t i = 0; i < add; ++i) {
    size_t pos = i * 2;
    char c1 = HexToByte(hex[pos], require_uppercase);
    char c2 = HexToByte(hex[pos + 1], require_uppercase);

    if (c1 < 0 || c2 < 0) return false;

    (*str)[from + i] = (c1 << 4) + c2;
  }

  return true;
}

}  // namespace


const string CEscape(const strings::cord& str) {
  string out;
  CEncode(str, &out);
  return out;
}

void CEncode(const strings::cord& str, string *out) {
  out->clear();
  for (size_t i = 0; i < str.length(); ++i) {
    char c = str[i];
    switch (c) {
      case '\0': out->append("\\000", 4); continue;
      case '\x1b': out->append("\\e", 2); continue;
      case '\a': out->append("\\a", 2); continue;
      case '\b': out->append("\\b", 2); continue;
      case '\t': out->append("\\t", 2); continue;
      case '\n': out->append("\\n", 2); continue;
      case '\v': out->append("\\v", 2); continue;
      case '\f': out->append("\\f", 2); continue;
      case '\r': out->append("\\r", 2); continue;
      case '\'': out->append("\\'", 2); continue;
      case '\"': out->append("\\\"", 2); continue;
      case '\?': out->append("\\?", 2); continue;
      case '\\': out->append("\\\\", 2); continue;
      default: break;
    }
    if (c < 0) {
      // Unicode?
      size_t len = Utf8LengthFromBuffer(str.ptr() + i, str.length() - i);
      if (len > 0) {
        int32_t cp = Utf8ToCodepoint(str.ptr() + i, len);
        if (cp > 0) {
          AppendEncodedUnicode(cp, out);
          i += len - 1;
          continue;
        }
      }
    }

    if (IsPrintable(c)) {
      out->append(&c, 1);
    } else {
      AppendBackslashOctal(c, out);
    }
  }
}


bool CDecode(const strings::cord& str, string *out) {
  out->clear();
  size_t done = 0;
  strings::cord tmp;
  for (size_t i = 0; i < str.length(); ++i) {
    if (!IsPrintable(str[i])) {
      return false;
    }
    if (str[i] == '\\') {
      if (i > done) {
        out->append(str.ptr() + done, i - done);
      }
      ++i;
      if (i >= str.length()) {
        return false;  // ends in '\' w/o fulfilled char.
      }
      switch (str[i]) {
        case 'a': out->append("\a", 1); break;
        case 'b': out->append("\b", 1); break;
        case 'e': out->append("\x1b", 1); break;
        case 't': out->append("\t", 1); break;
        case 'n': out->append("\n", 1); break;
        case 'v': out->append("\v", 1); break;
        case 'f': out->append("\f", 1); break;
        case 'r': out->append("\r", 1); break;
        case '\'': out->append("\'", 1); break;
        case '\"': out->append("\"", 1); break;
        case '\?': out->append("\?", 1); break;
        case '\\': out->append("\\", 1); break;
        case 'x':
          if (!((i + 1) < str.length())) {
            return false;  // str with single char.
          }
          tmp.reset(str.ptr() + i, 2);
          if (!AppendHexDecodeInternal(tmp, out, false)) {
            return false;  // Invlaid str string.
          }
          ++i;
          break;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7': {
            int ord = 0;
            size_t n = 0;
            for (; n < 3 && (i + n) < str.length() &&
                 str[i + n] >= '0' && str[i + n] <= '7';
                 ++n) {
              int oct = ord * 8 + (str[i + n] - '0');
              if (oct > 255) break;
              ord = oct;
            }
            char c = static_cast<char>(ord);
            out->append(&c, 1);
            i += n - 1;
          }
          break;
        default:
          return false;
      }
      done = i + 1;
    }
  }
  if (done < str.length()) {
    out->append(str.ptr() + done, str.length() - done);
  }
  return true;
}


void HexEncode(const strings::cord &data, string *onto) {
  onto->clear();
  AppendHexEncodeInternal(data, onto, 'a');
}


bool HexDecode(const strings::cord& data, string *onto) {
  onto->clear();
  return AppendHexDecodeInternal(data, onto, false);
}


void AppendHexEncode(const strings::cord &data, string *onto) {
  AppendHexEncodeInternal(data, onto, 'a');
}

bool AppendHexDecode(const strings::cord &hex, string *str) {
  return AppendHexDecodeInternal(hex, str, false);
}



void UrlEncode(const strings::cord& str, string *encoded) {
  encoded->clear();
  for (size_t i = 0; i < str.length(); ++i) {
    if (str[i] == ' ') {
      encoded->append("+", 1);
    } else if (!IsPrintable(str[i]) ||
        IsUrlReservedChar(str[i]) || str[i] == '%') {
      encoded->append("%", 1);
      strings::cord tmp(str.ptr() + i, 1);
      // URL percent encoded values require uppercase letters.
      AppendHexEncodeInternal(tmp, encoded, 'A');
    } else {
      encoded->append(str.ptr() + i, 1);
    }
  }
}


bool UrlDecode(const strings::cord& encoded, string *str) {
  str->clear();
  for (size_t i = 0; i < encoded.length(); ++i) {
    if (encoded[i] == '+') {
      str->append(" ", 1);
    } else if (encoded[i] == '%') {
      // percent encoded char.
      if (i + 3 > encoded.length()) {
        // Not enough chars to parse.
        return false;
      }
      strings::cord tmp(encoded.ptr() + i + 1, 2);
      string out;
      // URL percent encoded values require uppercase letters.
      if (!AppendHexDecodeInternal(tmp, &out, true)) {
        // not valid hex.
        return false;
      }
      i += 2;
      str->append(out);
    } else if (!IsPrintable(encoded[i]) ||
        IsUrlReservedChar(encoded[i])) {
      return false;
    } else {
      str->append(encoded.ptr() + i, 1);
    }
  }

  return true;
}


void Base64Encode(const strings::cord& data, bool web_safe, string *base64) {
  base64->clear();
  const char *table = 
          web_safe ? kWebSafeBase64ReplaceTable : kBase64ReplaceTable;
  for (size_t pos = 0; pos < data.length(); pos += 3) {
    strings::cord block(data.ptr() + pos,
        min(data.length() - pos, static_cast<size_t>(3)));
    Base64EncodeBlock(block, table, base64);
  }
  while (!web_safe && base64->size() % 4 != 0) {
    base64->append(&kBase64PadChar, 1);
  }
}


bool Base64Decode(const strings::cord& base64, bool web_safe, string *data) {
  data->clear();
  size_t len = base64.length();
  if (len == 0) {
    // Yes, the empty string is valid base64.
    return true;
  }

  const char *table = 
          web_safe ? kWebSafeBase64Reverse : kBase64Reverse;
  strings::cord non_padded(base64);

  // Only remove padding if the resulting string is padded to the correct size.
  if (len % 4 == 0) {
    // Sometimes the data can be quite huge. Reserve memory for the result to
    // ansure minimal number of memory allocs. WebSafe strings should be quite
    // short, and therefor no reservation is made.
    data->reserve((len / 4) * 3);
    if (non_padded[len - 1] == kBase64PadChar) {
      if (non_padded[len - 2] == kBase64PadChar) {
        non_padded.reset(non_padded.ptr(), len - 2);
      } else {
        non_padded.reset(non_padded.ptr(), len - 1);
      }
    }
  } else if (!web_safe) {
    return false;
  }

  for (size_t pos = 0; pos < non_padded.length(); pos += 4) {
    strings::cord block(non_padded.ptr() + pos,
        min(non_padded.length() - pos, static_cast<size_t>(4)));
    if (!Base64DecodeBlock(block, table, data)) {
      return false;
    }
  }

  return true;
}

}  // namespace sfu
