#include <iostream>

#include "sfu/console/interactive.h"
#include "sfu/encoding.h"



int main(int argc, char * argv[]) {
  sfu::console::LineReader reader("O'really?", NULL, NULL);
  std::string line("");
  if (reader.read(&line)) {
    std::cout << "You gave the line: \"" << line << "\"" << std::endl;
  } else {
    std::cout << "Aborted..." << std::endl;
  }
}
