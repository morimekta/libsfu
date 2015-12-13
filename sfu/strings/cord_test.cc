
#include <utility>
#include <iostream>

#include "sfu/strings/cord.h"
#include "gtest/gtest.h"

using namespace std;

TEST(CordTest, TestBasics) {
  string a = "abcdefgh";
  sfu::strings::cord b(a);
  sfu::strings::cord c(a.c_str()+2, 3);

  swap(b, c);

  // The values are swapped.
  EXPECT_EQ("abcdefgh", c.as_string());
  EXPECT_EQ("cde", b.as_string());
}
