#include <functional>

#include "gtest/gtest.h"

#include "sfu/console/argument_parser.h"
#include "sfu/numbers.h"

using namespace std;
using namespace sfu::console;

TEST(ArgumentParserTest, TestBasics) {
  ArgumentParser parser("gt", "Small command.");

  vector<string> test;
  test.push_back("test");
  test.push_back("--help");

  EXPECT_FALSE(parser.show_help());
  EXPECT_TRUE(parser.Apply(test));
  EXPECT_TRUE(parser.show_help());
}

TEST(ArgumentParserTest, TestLongArguments) {
  int32_t u32 = 0;
  bool b = false;
  bool b2 = true;
  string s;
  vector<string> m;

  ArgumentParser parser("gt", "Small command.");
  parser.add("--u32",    "u", "Unsigned int.",
        bind(sfu::safe_str2int32, placeholders::_1, &u32, 10));
  parser.add("--bool",   "b", "Boolean with argument.",
        bind(sfu::safe_str2bool, placeholders::_1, &b));
  parser.add("--bool2",  "B", "Unsigned int.", NULL,
        bind(parser::SetBoolean, false, &b2));
  parser.add("--list",   "L", "List of strings.",
        bind(parser::AppendStringVector, placeholders::_1, &m));
  parser.add("--string", "S", "A strings.",
        bind(parser::SetString, placeholders::_1, &s));

  vector<string> test;
  test.push_back("gt test");
  test.push_back("--bool2");
  test.push_back("--bool");
  test.push_back("true");
  test.push_back("--u32");
  test.push_back("-6333");
  test.push_back("--list=item1");
  test.push_back("--string");
  test.push_back("str");
  test.push_back("--list");
  test.push_back("item2");

  EXPECT_TRUE(parser.Apply(test));

  EXPECT_EQ("str", s);
  EXPECT_EQ(-6333, u32);
  EXPECT_TRUE(b);
  EXPECT_FALSE(b2);

  ASSERT_EQ(2, m.size());
  EXPECT_EQ("item1", m[0]);
  EXPECT_EQ("item2", m[1]);
}

TEST(ArgumentParserTest, TestShortArguments) {
  int32_t u32 = 0;
  bool b = false;
  bool b2 = true;
  string s;
  vector<string> m;

  ArgumentParser parser("gt", "Small command.");

  parser.add("--u32",    "u", "Unsigned int.",
        bind(sfu::safe_str2int32, placeholders::_1, &u32, 10));

  parser.add("--bool",   "b", "Boolean with argument.", 
        bind(sfu::safe_str2bool, placeholders::_1, &b));

  parser.add("--bool2",  "B", "Unsigned int.", NULL,
        bind(parser::SetBoolean, false, &b2));

  parser.add("--list",   "L", "List of strings.",
        bind(parser::AppendStringVector, placeholders::_1, &m));

  parser.add("--string", "S", "A strings.",
        bind(parser::SetString, placeholders::_1, &s));

  vector<string> test;
  test.push_back("gt test");
  test.push_back("-Bb");
  test.push_back("true");
  test.push_back("-u");
  test.push_back("-6333");
  test.push_back("-L");
  test.push_back("item1");
  test.push_back("-S");
  test.push_back("str");
  test.push_back("-L");
  test.push_back("item2");

  EXPECT_TRUE(parser.Apply(test));

  EXPECT_EQ("str", s);
  EXPECT_EQ(-6333, u32);
  EXPECT_TRUE(b);
  EXPECT_FALSE(b2);

  ASSERT_EQ(2, m.size());
  EXPECT_EQ("item1", m[0]);
  EXPECT_EQ("item2", m[1]);
}
