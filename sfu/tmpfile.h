#ifndef UTIL_TMPFILE_H_
#define UTIL_TMPFILE_H_

#include <string>
#include <vector>

namespace sfu {

class TmpFile {
  public:
    TmpFile(const std::string& path);
    ~TmpFile();

    const std::string Create(const std::string& suffix);

    void Cleanup();

  private:
    std::vector<std::string> files_;
    std::string prefix_;

    // DISALLOW_COPY_AND_ASSIGN
    TmpFile(const TmpFile& tmp) {}
    void operator=(const TmpFile& tmp) {}
};

}  // namespace sfu

#endif  // UTIL_TMPFILE_H_
