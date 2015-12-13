
#include "sfu/strings/format.h"
#include "gtest/gtest.h"

#include <ctime>

using namespace std;
using namespace sfu::strings;

TEST(FormatStringTest, TestFormatString) {
  EXPECT_EQ("11233B", format("1%iB", 1233));
}
