
#include "sfu/popen.h"
#include "gtest/gtest.h"

using namespace std;
using namespace sfu;

TEST(PopenTest, TestSimplePopen) {
  vector<string> cmd;
  cmd.push_back("echo");
  cmd.push_back("popen-test");
  Popen popen(cmd);
  EXPECT_GT(popen.pid(), 0);
  EXPECT_EQ(popen.wait(), 0);
  EXPECT_EQ(popen.pid(), 0);
}

TEST(PopenTest, TestPopenWithOutputPipe) {
  vector<string> cmd;
  cmd.push_back("echo");
  cmd.push_back("test");
  cmd.push_back("string");
  Popen popen(cmd, Popen::PIPE);
  char buffer[256] = {0, };
  size_t p = 0;
  string out;
  while ((p = read(popen.pout(), buffer, 255)) > 0) {
    buffer[p] = 0;
    out.append(buffer);
  }

  // There is no perr pipe.
  EXPECT_GT(popen.pout(), 2);
  EXPECT_EQ(popen.perr(), -1);
  EXPECT_EQ(popen.wait(), 0);

  EXPECT_EQ(out, "test string\n");
}


TEST(PopenTest, TestPopenWithErrorPipe) {
  vector<string> cmd;
  cmd.push_back("ls");
  cmd.push_back("--garibaldi");
  Popen popen(cmd, Popen::TIE, Popen::PIPE);
  char buffer[256] = {0, };
  size_t p = 0;
  string err;
  while ((p = read(popen.perr(), buffer, 255)) > 0) {
    buffer[p] = 0;
    err.append(buffer);
  }

  EXPECT_GT(popen.perr(), 2);
  EXPECT_EQ(popen.pout(), -1);

  popen.wait();

  EXPECT_GT(err.size(), 40);
}

TEST(PopenTest, TestPopenRun) {
  string out, err;

  vector<string> cmd;
  cmd.push_back("ls");
  cmd.push_back("--garibaldiiii");

  Popen::run(cmd, &out, &err);
  EXPECT_EQ(out.size(), 0);
  EXPECT_GT(err.size(), 40);
}
