
#include "sfu/numbers.h"
#include "gtest/gtest.h"

using namespace std;
using namespace sfu;

TEST(NumbersTest, TestParseBoolOK) {
  bool b = false;
  EXPECT_TRUE(safe_str2bool("true", &b));
  EXPECT_TRUE(b);
  EXPECT_TRUE(safe_str2bool("false", &b));
  EXPECT_FALSE(b);
  EXPECT_TRUE(safe_str2bool("TRUE", &b));
  EXPECT_TRUE(b);
  EXPECT_TRUE(safe_str2bool("FALSE", &b));
  EXPECT_FALSE(b);
  EXPECT_TRUE(safe_str2bool("yes", &b));
  EXPECT_TRUE(b);
  EXPECT_TRUE(safe_str2bool("no", &b));
  EXPECT_FALSE(b);
  EXPECT_TRUE(safe_str2bool("Yes", &b));
  EXPECT_TRUE(b);
  EXPECT_TRUE(safe_str2bool("No", &b));
  EXPECT_FALSE(b);
  EXPECT_TRUE(safe_str2bool("Y", &b));
  EXPECT_TRUE(b);
  EXPECT_TRUE(safe_str2bool("n", &b));
  EXPECT_FALSE(b);
  EXPECT_TRUE(safe_str2bool("t", &b));
  EXPECT_TRUE(b);
  EXPECT_TRUE(safe_str2bool("f", &b));
  EXPECT_FALSE(b);
  EXPECT_TRUE(safe_str2bool("1", &b));
  EXPECT_TRUE(b);
  EXPECT_TRUE(safe_str2bool("0", &b));
  EXPECT_FALSE(b);
}

TEST(NumbersTest, TestParseBoolFail) {
  bool b = false;
  EXPECT_FALSE(safe_str2bool("", &b));
  EXPECT_FALSE(safe_str2bool("yeah", &b));
  EXPECT_FALSE(safe_str2bool("yalla", &b));
  EXPECT_FALSE(safe_str2bool("falser", &b));
  EXPECT_FALSE(safe_str2bool("-1", &b));
  EXPECT_FALSE(safe_str2bool("0000", &b));
  EXPECT_FALSE(safe_str2bool("1000", &b));
}

TEST(NumbersTest, TestParseDouble) {
  float f = 0;
  double d = 0;
  // long double ld = 0;

  EXPECT_TRUE(safe_str2f("-22.76", &f));
  EXPECT_FLOAT_EQ(-22.76, f);

  EXPECT_TRUE(safe_str2d("4755667788.8000002", &d));
  EXPECT_DOUBLE_EQ(4755667788.8000002, d);
}
