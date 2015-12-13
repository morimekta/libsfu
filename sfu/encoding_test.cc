#include "sfu/encoding.h"
#include "gtest/gtest.h"

using namespace sfu;
using namespace std;

TEST(EncodingTest, TestHexEncode) {
  string raw = "This is a simple test.";

  string encoded;
  HexEncode(raw, &encoded);
  EXPECT_EQ(encoded, "5468697320697320612073696d706c6520746573742e");

  string decoded;
  HexDecode(encoded, &decoded);
  EXPECT_EQ(raw, decoded);

  string comp = "Th\031\"\b";
  HexEncode(comp, &encoded);
  EXPECT_EQ(encoded, "5468192208");

  HexDecode(encoded, &decoded);
  EXPECT_EQ(comp, decoded);
}

TEST(EncodingTest, TestCEncode) {
  string raw = "\a\b gfsdj \x1c\031, &;?.\"\'";
  string encoded = CEscape(raw);

  EXPECT_EQ(encoded, "\\a\\b gfsdj \\034\\031, &;\\?.\\\"\\\'");

  string decoded;
  CDecode(encoded, &decoded);

  EXPECT_EQ(raw, decoded);
}

TEST(EncodingTest, TestUrlEncode) {
  string raw = "\a\b gfsdj \x1c\031, &;?.\"\'";
  string encoded;
  UrlEncode(raw, &encoded);

  EXPECT_EQ(encoded, "%07%08+gfsdj+%1C%19%2C+%26%3B%3F.%22%27");

  string decoded;
  UrlDecode(encoded, &decoded);

  EXPECT_EQ(raw, decoded);
}

TEST(EncodingTest, TestBase64Encode) {
  string raw = "Zis is a long test.";
  string encoded;
  string decoded;

  Base64Encode(raw, false, &encoded);

  EXPECT_EQ(encoded, "WmlzIGlzIGEgbG9uZyB0ZXN0Lg==");

  Base64Decode(encoded, false, &decoded);

  EXPECT_EQ(raw, decoded);
}
