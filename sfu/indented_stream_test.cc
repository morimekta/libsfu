
#include "sfu/indented_stream.h"
#include "gtest/gtest.h"

using namespace std;
using namespace sfu;

TEST(NumbersTest, TestSimpleStream) {
  stringstream ss;
  IndentedStream is(&ss);

  is << "void a() {\n";
  is.begin();
  is << "return;" << endl;
  is.end();
  is << '}' << flush;

  ASSERT_EQ("void a() {\n"
            "    return;\n"
            "}", ss.str());
}
