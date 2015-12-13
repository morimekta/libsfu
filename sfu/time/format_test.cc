#include "sfu/time/format.h"
#include "gtest/gtest.h"

using namespace sfu::time;


TEST(FormatStringTest, TestFormatStringTime) {
  time_t ts = 1234567890;

  const char *old_tz = getenv("TZ");
  setenv("TZ", "America/Los_Angeles", 1);
  tzset();

  EXPECT_EQ("2009-02-13 15:31:30 PST", FormatStringTime("%F %T %Z", ts));
  EXPECT_EQ("2009-02-13 23:31:30 GMT", FormatStringTime("%F %T %Z", ts, 1));

  setenv("TZ", "Asia/Kolkata", 1);
  tzset();

  EXPECT_EQ("2009-02-14 05:01:30 IST", FormatStringTime("%F %T %Z", ts));

  setenv("TZ", old_tz, 1);
  tzset();
}
