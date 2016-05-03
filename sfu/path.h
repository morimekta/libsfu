#ifndef SFU_PATH_H_
#define SFU_PATH_H_

#include <string>

namespace sfu {

static const char kPathSeparationChar = '/';

extern const char* const kPathSubdir;

extern const char* const kPathSameDir;

// Get the current working directory path (PWD).
const std::string CurrentWorkingDirectory();

// Checks if a path string is a valid (and safe) path. This disallows utf-8
// chars, control chars, URI encoded chars (%..): So ASCII letters, numbers,
// ".:-_=@" for file name. '.' and '..' are reserved special dirs. Separation
// char '/' for separation.
bool IsValidPath(const std::string& path);

// Checks a path is absolute and is entirely consistent (does not contain '.'
// or '..' special dirs).
bool IsConsistentAbsolutePath(const std::string& path);

// Consolidate the path so it contains as few elements as possible. Any
// consolidated absolute paths should be unique and stable for each file.
bool ConsolidatePath(std::string* path);

// Get consistent absolute path either from current working directory (PWD) or
// from given relative path.
const std::string AbsolutePath(const std::string& path);
const std::string AbsolutePath(const std::string& path,
                               const std::string& relative_to);

// Get the relative path of an absolute path file from the current working
// directory (PWD) or provided relative to path.
const std::string RelativePath(const std::string& absolute_path);
const std::string RelativePath(const std::string& absolute_path,
                               const std::string& relative_from);



}  // namespace sfu

#endif  // SFU_PATH_H_
