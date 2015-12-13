#include <cstdio>
#include <cstdlib>
#include <ctime>

#include "sfu/tmpfile.h"
#include "sfu/encoding.h"
#include "sfu/strings/cord.h"

namespace sfu {

TmpFile::TmpFile(const std::string& path) {
  srand(time(NULL));
  uint8_t rnd = rand() % 256;

  std::string tmp;
  HexEncode(strings::cord(reinterpret_cast<const char*>(&rnd), sizeof(uint8_t)), &tmp);
  prefix_.append(path);
  prefix_.append(".tmp.");
  prefix_.append(tmp);
  prefix_.append(".");
}

TmpFile::~TmpFile() {
  Cleanup();
}

const std::string TmpFile::Create(const std::string& suffix) {
  uint32_t rnd = rand();

  std::string pre;
  Base64Encode(strings::cord(reinterpret_cast<const char*>(&rnd), sizeof(uint32_t)),
      true, &pre);
  std::string filename(prefix_);
  filename.append(pre);
  filename.append(".", 1);
  filename.append(suffix);
  files_.push_back(filename);

  return filename;
}

void TmpFile::Cleanup() {
  for (std::string file : files_) {
    if (remove(file.c_str()) != 0) {
      // error removing file.
    }
  }
  files_.clear();
}

}  // namespace sfu
