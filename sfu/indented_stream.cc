#include "sfu/indented_stream.h"

using namespace std;

namespace sfu {
namespace {

static const char* const kDefaultIndent = "    ";

string make_indent(const vector<string>& stack) {
  string out;
  for (vector<string>::const_iterator it = stack.begin();
       it != stack.end();
       ++it) {
    out.append(*it);
  }
  return out;
}

}  // namespace

IndentedStream::IndentedStream(ostream* out) :
  std::ios(0), std::basic_ostream<char>(0), out_(out) {

}
IndentedStream::~IndentedStream() {}

IndentedStream& IndentedStream::begin() {
  return begin(kDefaultIndent);
}

IndentedStream& IndentedStream::begin(const string& indent) {
  stack_.push_back(indent);
  indent_ = make_indent(stack_);
  return *this;
}

IndentedStream& IndentedStream::end() {
  if (stack_.size() > 0) {
    stack_.pop_back();
    indent_ = make_indent(stack_);
  }
  return *this;
}

IndentedStream& IndentedStream::put(char c) {
  if (c == '\n') {
    clear_line_ = true;
  } else {
    maybe_indent();
  }
  out_->put(c);
  return *this;
}

IndentedStream& IndentedStream::put(const console::Char& c) {
  if (c == console::Char::NEWLINE) {
    clear_line_ = true;
  } else {
    maybe_indent();
  }
  (*out_) << c.c_str();
  return *this;
}

IndentedStream& IndentedStream::write(const std::string& str) {
  if (str.size() > 0) {
    if (str == "\n") return put('\n');
    maybe_indent();
    (*out_) << str;
    if (str[str.size() - 1] == '\n') {
      clear_line_ = true;
    }
  }
  return *this;
}

IndentedStream& IndentedStream::write(const char* const str) {
  return write(string(str));
}

IndentedStream& IndentedStream::write(const char* const str) {
  return write(string(str));
}


IndentedStream& IndentedStream::newline() {
  (*out_) << '\n';
  clear_line_ = true;
  return *this;
}

void IndentedStream::maybe_indent() {
  if (clear_line_) {
    (*out_) << indent_;
    clear_line_ = false;
  }
}


}  // namespace sfu
