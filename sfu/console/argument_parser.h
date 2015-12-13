#ifndef CONSOLE_ARGUMENT_PARSER_H_
#define CONSOLE_ARGUMENT_PARSER_H_

#include <functional>
#include <map>
#include <ostream>
#include <string>
#include <vector>

#include "sfu/strings/cord.h"

namespace sfu {
namespace console {

class Option {
 public:
  Option(const sfu::strings::cord& name,
         const sfu::strings::cord& short_name,
         const sfu::strings::cord& help,
         std::function<bool(const sfu::strings::cord&)> parser,
         std::function<bool()> noarg_parser = NULL); 
  ~Option();

  const std::string& name() const;
  const std::string& short_name() const;
  const std::string& help() const;
  const std::string& metavar() const;
  const std::string& metaname() const;
  const std::string& default_value() const;
  bool               hidden() const;
  char               num_args() const;

  Option* set_metavar(const sfu::strings::cord& metavar);
  Option* set_metaname(const sfu::strings::cord& metaname);
  Option* set_default_value(const sfu::strings::cord& defval);
  Option* set_hidden();
  Option* set_multi_args();

  // If the option requires an argument.
  bool RequiresArgument() const;
  // If the option can accept an argument, but also none. This care the
  // argument requires the use of the "--option=argument" syntax. Also does not
  // support the multiple arguments syntax.
  bool OptionalArgument() const;
  // The option takes multiple consecutive arguments. This flag implies
  // RequiresArgument.
  bool MultipleArguments() const;

  bool Apply(std::ostream* os, const sfu::strings::cord& arg);
  bool Apply(std::ostream* os);

 private:
  std::string name_;
  std::string short_name_;
  std::string help_;
  std::string metavar_;
  std::string metaname_;
  std::string default_value_;

  bool hidden_;
  char num_args_;

  std::function<bool(const sfu::strings::cord&)> parser_;
  std::function<bool()> noarg_parser_;
};


class ArgumentParser {
 public:
  ArgumentParser(const sfu::strings::cord& command,
                 const sfu::strings::cord& description,
                 bool add_default_help = true);
  ~ArgumentParser();

  bool Apply(int argc, char **argv);
  bool Apply(const std::vector<std::string>& args);

  void PrintHelp(std::ostream* os) const;

  bool show_help() const { return show_help_; }

  Option* add(const sfu::strings::cord& name,
              const sfu::strings::cord& short_name,
              const sfu::strings::cord& help,
              std::function<bool(const sfu::strings::cord&)> parser,
              std::function<bool()> noarg_parser = NULL);

 protected:
  Option* add(Option* opt);

 private:
  typedef std::vector<std::string>::const_iterator vs_iterator;
  bool show_help_;

  Option* find(const sfu::strings::cord& name);
  Option* find_short(char c);

  bool ApplyInternal(const std::vector<std::string>& args);
  bool ApplyShortOption(vs_iterator& i, const vs_iterator& end);
  bool ApplyLongOption (vs_iterator& i, const vs_iterator& end);
  bool ApplyOperand    (vs_iterator& i, const vs_iterator& end, bool* end_operands);

  // The command run, or specified in the constructor.
  std::string command_;
  // Description of the program.
  std::string description_;
  // Usage string to replace the default generated one.
  std::string usage_;

  // Ordered list of options, used for printing the help page.
  std::vector<Option*> options_order_;

  // All other arguments (or all after -- is an operand). The first operand
  // that accepts a value will get it.
  std::vector<Option*> operands_;

  // long name to option map.
  std::map<sfu::strings::cord, Option*> options_;

  // short name to option map.
  std::map<char, Option*> short_opts_;

  // Output stream to print output to. Default is std::cerr.
  std::ostream* os_;
};


namespace parser {

// Helper functions to set values that are not really parsed. Otherwise the
// safe_* functions from sfu/numbers.h can be used for numbers and parsing the
// bool value. E.g.:
//
// std::bind(sfu::safe_str2uint32, std::placeholders::_1, value);

bool SetBoolean(bool set_to, bool* value);
// std::bind(parser::SetBoolean, true, value);
bool SetString(const sfu::strings::cord& str, std::string *value);
// std::bind(parser::SetString, std::placeholders::_1, value);
bool AppendStringVector(const sfu::strings::cord& str,
                        std::vector<std::string>* value);
// std::bind(parser::AppendStringVector, std::placeholders::_1, value);

}  // namespace parser
}  // namespace console
}  // namespace sfu

#endif  // CONSOLE_ARGUMENT_PARSER_H_
