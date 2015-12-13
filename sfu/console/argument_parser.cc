
#include <algorithm>

#include "sfu/console/argument_parser.h"
#include "sfu/nullstream.h"
#include "sfu/numbers.h"
#include "sfu/strings/cord.h"

using namespace std;

namespace sfu {
namespace console {
namespace {
  // Helpers...
}

Option::Option(const strings::cord& name,
               const strings::cord& short_name,
               const strings::cord& help,
               std::function<bool(const strings::cord&)> parser,
               std::function<bool()> noarg_parser) :
      name_(name.ptr(), name.length()),
      short_name_(short_name.ptr(), short_name.length()),
      help_(help.ptr(), help.length()),
      hidden_(false),
      num_args_(parser ? (noarg_parser ? '?' : '1') : '0'),
      parser_(parser),
      noarg_parser_(noarg_parser) {}
Option::~Option() {}

const string& Option::name()        const { return name_; }
const string& Option::short_name()  const { return short_name_; }
const string& Option::help()        const { return help_; }
const string& Option::metavar()     const { return metavar_; }
const string& Option::metaname()    const { return metaname_; }
bool          Option::hidden()      const { return hidden_; }
char          Option::num_args()    const { return num_args_; }

Option* Option::set_metavar(const sfu::strings::cord& metavar) {
  metavar_ = string(metavar.ptr(), metavar.length());
  return this;
}

Option* Option::set_metaname(const sfu::strings::cord& metaname) {
  metaname_ = string(metaname.ptr(), metaname.length());
  return this;
}

Option* Option::set_default_value(const sfu::strings::cord& defval) {
  default_value_ = string(defval.ptr(), defval.length());
  return this;
}

Option* Option::set_hidden() {
  hidden_ = true;
  return this;
}

Option* Option::set_multi_args() {
  // assert (parser_)
  num_args_ = '+';
  noarg_parser_ = NULL;  // Incompatible, this simplifies the paring code a *lot*.
  return this;
}


bool Option::Apply(std::ostream* os, const sfu::strings::cord& arg) {
  if (!parser_) {
    (*os) << "Option \"" << name_ << "\" forbids argument, but given \""
          << arg << "\"." << endl;
    return false;
  }
  if (!parser_(arg)) {
    (*os) << "Option \"" << name_ << "\" given an invalid"
           << " argument. [" << arg << "]." << endl;
    return false;
  }
  return true;
}


bool Option::Apply(std::ostream* os) {
  if (!noarg_parser_) {
    (*os) << "Option \"" << name_ << "\" requires argument, but not given." << endl;
    return false;
  }
  if (!noarg_parser_()) {
    (*os) << "Failed to apply option \"" << name_ << "\"." << endl;
    return false;
  }
  return true;
}

bool Option::RequiresArgument() const {
  return (parser_ && !noarg_parser_);
}

bool Option::OptionalArgument() const {
  return (parser_ && noarg_parser_);
}

bool Option::MultipleArguments() const {
  return num_args_ == '+';
}

// ----------------------------------------------
// ---          ArgumentParser class          ---
// ----------------------------------------------

ArgumentParser::ArgumentParser(const strings::cord& command,
                               const strings::cord& description,
                               bool add_default_help)
  : show_help_(false),
    command_(command.ptr(), command.length()),
    description_(description.ptr(), description.length()),
    os_(&std::cerr) {
  if (add_default_help) {
    add(new Option("--help", "h", "Shows this help page.", NULL,
                   std::bind(parser::SetBoolean, true, &show_help_)));
  }
}

ArgumentParser::~ArgumentParser() {
  for (Option* opt : options_order_) {
    delete opt;
  }
  for (Option* opt : operands_) {
    delete opt;
  }
}

Option* ArgumentParser::add(Option* opt) {
  if (opt->name()[0] == '-') {
    // Option.
    options_[strings::cord(opt->name())] = opt;
    for (char c : opt->short_name()) {
      short_opts_[c] = opt;
    }
  } else {
    // Operand.
    operands_.push_back(opt);
  }
  return opt;
}

Option* ArgumentParser::add(
    const sfu::strings::cord& name,
    const sfu::strings::cord& short_name,
    const sfu::strings::cord& help,
    std::function<bool(const sfu::strings::cord&)> parser,
    std::function<bool()> noarg_parser) {
  return add(new Option(name, short_name, help, parser, noarg_parser));
}


bool ArgumentParser::Apply(int argc, char ** argv) {
  vector<string> args;
  for (int i = 0; i < argc; ++i) {
    args.push_back(argv[i]);
  }
  return Apply(args);
}


bool ArgumentParser::Apply(const vector<string>& args) {
  if (!ApplyInternal(args) || show_help_) {
    PrintHelp(os_);
    return show_help_;
  }
  return true;
}

void ArgumentParser::PrintHelp(std::ostream* os) const {
  // Print cmd usage
  //     description
  //
  // Available options:
  //
  // Available operands:
}

bool ArgumentParser::ApplyShortOption(
    vs_iterator& i, const vs_iterator& end) {
  size_t len = i->size();
  for (size_t j = 1; j < len; ++j) {
    char c = i->at(j);
    Option *opt = find_short(c);
    if (!opt) {
      // No such option.
      (*os_) << "II: No such option \'" << c << "\'." << endl;
      return false;
    }
    bool last = (j == (len - 1));
    if (last && opt->RequiresArgument()) {
      if ((i + 1) == end) {
        // No argument when required.
        (*os_) << "Option \"" << opt->name() << "\" from \'" << c
               << "\' requires an argument." << endl;
        return false;
      }

      if (!opt->Apply(os_, *(++i))) {
        return false;
      }

    } else if (!opt->Apply(os_)) {
      return false;
    }
  }

  return true;
}

bool ArgumentParser::ApplyLongOption(
    vs_iterator& i, const vs_iterator& end) {
  strings::cord name(*i);
  strings::cord value;

  bool has_arg = false;
  size_t eq = name.find('=');
  if (eq != strings::cord::npos) {
    value.reset(name.ptr() + eq + 1, name.length() - eq - 1);
    name.reset(name.ptr(), eq);
    has_arg = true;
  }
  Option* opt = find(name);
  if (!opt) {
    (*os_) << "No such option \"" << name << "\"." << endl;
    return false;
  }

  if (eq == strings::cord::npos) {
    if (opt->RequiresArgument()) {
      if ((++i) == end) {
        (*os_) << "Option \"" << opt->name()
               << "\" requires an argument." << endl;
        return false;
      }
      value.reset(*i);
      has_arg = true;
    }

    if (opt->MultipleArguments()) {
      if (!opt->Apply(os_, value)) {
        return false;
      }
      // Look ahead and maybe consume another argument.
      while ((i + 1) != end) {
        value.reset(*(i + 1));
        if (value.string_equals("-") || value.string_equals("--")) return true;
        // If the value looks like a flag (begins with a '-' and is not a valid
        // number) parse it as the next flag.
        int64_t tmp_int;
        double tmp_dbl;
        if (value.length() > 0 && value[0] == '-' &&
            !safe_str2int64(value, &tmp_int) &&
            !safe_str2d(value, &tmp_dbl)) {
          // This is a new flag.
          return true;
        }
        if (!opt->Apply(os_, value)) {
          return false;
        }
        ++i;
      }
      // We've parsed it all... 
      return true;
    }
  }

  if (has_arg) {
    if (!opt->Apply(os_, value)) {
      return false;
    }
  } else {
    if (!opt->Apply(os_)) {
      return false;
    }
  }

  return true;
}

bool ArgumentParser::ApplyOperand(
    vs_iterator& i, const vs_iterator& end, bool* end_options) {
  for (Option* operand : operands_) {
    if (operand->Apply(&sfu::nout, *i)) {
      if (operand->num_args() == '+') {
        *end_options = true;
      }
      return true;
    }
  }

  (*os_) << "Operand not recognized: \"" << *i << "\"." << endl;
  return false;
}

bool ArgumentParser::ApplyInternal(const vector<string>& args) {
  // assert(args.size() > 0)
  auto i = args.cbegin();
  if (command_.size() == 0) {
    command_ = *i;
  }

  bool end_options = false;
  while (++i != args.cend()) {
    if (i->size() == 0) {
      (*os_) << "The empty string is not a valid option.";
      return false;
    }

    if (i->at(0) == '-') {
      if (i->size() > 1 && i->at(1) == '-') {
        if (i->size() > 2) {
          // Long form.
          if (!ApplyLongOption(i, args.cend())) return false;
        } else {
          end_options = true;
        }
      } else if(i->size() > 1) {
        if (!ApplyShortOption(i, args.cend())) return false;
      } else {
        end_options = true;
      }
    } else {
      if (!ApplyOperand(i, args.cend(), &end_options)) return false;
    }
  }
  return true;
}


Option *ArgumentParser::find(const strings::cord& name) {
  auto p = options_.find(name);
  if (p == options_.end()) return NULL;
  return p->second;
}

Option* ArgumentParser::find_short(char c) {
  auto p = short_opts_.find(c);
  if (p == short_opts_.end()) return NULL;
  return p->second;
}


namespace parser {

bool SetBoolean(bool set_to, bool* value) {
  (*value) = set_to;
  return true;
}

bool SetString(const strings::cord& str, std::string* value) {
  (*value) = str.as_string();
  return true;
}

bool AppendStringVector(const strings::cord& str, std::vector<std::string>* value) {
  value->push_back(str.as_string());
  return true;
}

}  // namespace parser
}  // namespace console
}  // namespace sfu
