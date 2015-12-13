#include "gtest/gtest.h"

#include "sfu/console/char.h"
#include "sfu/encoding.h"

using namespace std;
using namespace sfu::console;

TEST(CharTest, TestConstructor) {
  char buffer[] = "\u2018";
  string str("\u2018");

  Char b(0x2018);
  Char c(b);
  Char d(buffer);
  Char e(buffer, 3);
  Char f(str);
  Char g = f;

  EXPECT_TRUE(b == c);
  EXPECT_TRUE(b == d);
  EXPECT_TRUE(b == e);
  EXPECT_TRUE(b == f);
  EXPECT_TRUE(b == g);

  EXPECT_EQ(0x2018, b.codepoint());
  EXPECT_EQ(0x2018, c.codepoint());
  EXPECT_EQ(0x2018, d.codepoint());
  EXPECT_EQ(0x2018, e.codepoint());
  EXPECT_EQ(0x2018, f.codepoint());
  EXPECT_EQ(0x2018, g.codepoint());
}

TEST(CharTest, TestColors) {
}
