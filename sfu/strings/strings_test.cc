#include <string>
#include <vector>
#include <map>
#include <deque>

#include "sfu/strings/strings.h"
#include "gtest/gtest.h"

using namespace sfu::strings;
using namespace std;

TEST(StringsTest, SplitInto) {
  vector<string> result;
  ASSERT_EQ(2, split_into(&result, "abcde", "b"));
  ASSERT_EQ(2, result.size());
  EXPECT_EQ("a", result[0]);
  EXPECT_EQ("cde", result[1]);

  result.clear();
  ASSERT_EQ(4, split_into(&result, "ab,c,d,", ","));
  EXPECT_EQ(4, result.size());

  EXPECT_EQ("ab", result[0]);
  EXPECT_EQ("c", result[1]);
  EXPECT_EQ("d", result[2]);
  EXPECT_EQ("", result[3]);

  result.clear();
  ASSERT_EQ(3, split_into(&result, "ab,c,d,", ",", true));
  EXPECT_EQ(3, result.size());
  EXPECT_EQ("ab", result[0]);
  EXPECT_EQ("c", result[1]);
  EXPECT_EQ("d", result[2]);
}

TEST(StringsTest, SplitInto_Deque) {
  deque<string> result;
  ASSERT_EQ(2, split_into(&result, "abcde", "b"));
  ASSERT_EQ(2, result.size());
  EXPECT_EQ("a", result[0]);
  EXPECT_EQ("cde", result[1]);

  result.clear();
  ASSERT_EQ(4, split_into(&result, "ab,c,d,", ","));
  EXPECT_EQ(4, result.size());

  EXPECT_EQ("ab", result[0]);
  EXPECT_EQ("c", result[1]);
  EXPECT_EQ("d", result[2]);
  EXPECT_EQ("", result[3]);

  result.clear();
  ASSERT_EQ(3, split_into(&result, "ab,c,d,", ",", true));
  EXPECT_EQ(3, result.size());
  EXPECT_EQ("ab", result[0]);
  EXPECT_EQ("c", result[1]);
  EXPECT_EQ("d", result[2]);
}

TEST(StringsTest, Join) {
  // TODO(steineldar): Make test
  vector<string> a;

  EXPECT_EQ("", join(a.begin(), a.end(), ","));

  a.push_back("a");
  a.push_back("b");
  a.push_back("c");

  EXPECT_EQ("a,b,c", join(a.begin(), a.end(), ","));
}

TEST(StringsTest, JoinInto) {
  // TODO(steineldar): Make test
  vector<string> a;
  string out;

  EXPECT_EQ(0, join_into(&out, a.begin(), a.end(), ","));
  EXPECT_EQ("", out);

  a.push_back("a");
  a.push_back("b");
  a.push_back("c");

  EXPECT_EQ(3, join_into(&out, a.begin(), a.end(), ","));
  EXPECT_EQ("a,b,c", out);
}

TEST(StringsTest, SplitKVPCallback) {
  string cfg =
    "config=list\n"
    "with.various=values\n";

  string query = "query=string&is=different&empty";

  map<string, string> result;
  int calls = 0;
  ASSERT_EQ(2, split_kvp_cb(
        [&calls, &result](const string& key, const string& value) -> bool {
          result[key] = value;
          ++calls;
          return true;
        }, cfg, "\n", "=", true, false, false));
  EXPECT_EQ(2, calls);
  EXPECT_EQ(2, result.size());
  EXPECT_EQ("list", result["config"]);
  EXPECT_EQ("values", result["with.various"]);

  result.clear();
  calls = 0;

  ASSERT_EQ(3, split_kvp_cb(
        [&calls, &result](const string& key, const string& value) -> bool {
          result[key] = value;
          ++calls;
          return true;
        }, query, "&", "=", false, false, true));

  EXPECT_EQ(3, calls);
  EXPECT_EQ(3, result.size());
  EXPECT_EQ("string", result["query"]);
  EXPECT_EQ("different", result["is"]);
  EXPECT_EQ("", result["empty"]);

  result.clear();
  calls = 0;

  string query2 = "query=string&is=different&empty=";
  ASSERT_EQ(2, split_kvp_cb(
        [&calls, &result](const string& key, const string& value) -> bool {
          result[key] = value;
          ++calls;
          return true;
        }, query2, "&", "=", false, true, false));

  EXPECT_EQ(2, calls);
  EXPECT_EQ(2, result.size());
  EXPECT_EQ("string", result["query"]);
  EXPECT_EQ("different", result["is"]);
}

TEST(StringsTest, SplitKVPInto) {
}

// -----------------

TEST(StringsTest, HasPrefix) {
  EXPECT_FALSE(has_prefix("a/b/c/d", "c/d"));
  EXPECT_FALSE(has_prefix("a/b/c/d", "a/d"));
  EXPECT_TRUE (has_prefix("a/b/c/d", "a/b"));
}

TEST(StringsTest, HasSuffix) {
  EXPECT_FALSE(has_suffix("a/b/c/d", "a/b"));
  EXPECT_FALSE(has_suffix("a/b/c/d", "a/d"));
  EXPECT_TRUE (has_suffix("a/b/c/d", "c/d"));
}

TEST(StringsTest, StripPrefix) {
  string str("a/b/c/d");
  EXPECT_FALSE(strip_prefix(&str, "c/d"));
  EXPECT_EQ   ("a/b/c/d", str);
  EXPECT_FALSE(strip_prefix(&str, "a/d"));
  EXPECT_EQ   ("a/b/c/d", str);

  EXPECT_TRUE(strip_prefix(&str, "a/b"));
  EXPECT_EQ   ("/c/d", str);
}

TEST(StringsTest, StripSuffix) {
  string str("a/b/c/d");
  EXPECT_FALSE(strip_suffix(&str, "a/b"));
  EXPECT_EQ   ("a/b/c/d", str);
  EXPECT_FALSE(strip_suffix(&str, "a/d"));
  EXPECT_EQ   ("a/b/c/d", str);

  EXPECT_TRUE(strip_suffix(&str, "c/d"));
  EXPECT_EQ   ("a/b/", str);
}


TEST(StringsTest, TrimWhitespace) {
  auto inline_trim_whitespace = [](const string& str) {
    string out = str;
    trim_whitespace(&out);
    return out;
  };

  EXPECT_EQ("", inline_trim_whitespace(" \t\n"));
  EXPECT_EQ("a", inline_trim_whitespace("\n\n\n   a"));
  EXPECT_EQ("a", inline_trim_whitespace("a\r\n\r\n"));
  EXPECT_EQ("a     a", inline_trim_whitespace("\n\n\n   a     a\r\n\r\n"));
}
