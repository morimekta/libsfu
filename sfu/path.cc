#include "sfu/path.h"

#include <string>
#include <list>

#include "sfu/strings/strings.h"

using namespace std;

namespace sfu {

const char* const kPathSubdir = "..";

const char* const kPathSameDir = ".";

const string CurrentWorkingDirectory() {
  const char* pwd = getenv("PWD");
  if (pwd) {
    return pwd;
  }
  return "/";
}

bool IsValidPath(const string& path) {
  return false;
}

bool IsValidFileName(const string& name) {
  if (name.empty() || name == kPathSameDir || name == kPathSubdir) {
    return false;
  }
  if (name.find("/") != string::npos) return false;
  return true;
}

bool IsConsistentAbsolutePath(const std::string& path) {
  if (path.empty()) return false;
  if (path[0] != kPathSeparationChar) return false;
  list<string> components;
  sfu::strings::split_into(&components,
      path, {kPathSeparationChar, '\0'}, false);
  for (string component : components) {
    if (!IsValidFileName(component)) return false;
  }
  return true;
}

bool ConsolidatePath(string* path) {
  if (!path || path->empty()) return false;

  list<string> out;
  bool isAbsolute = false;

  list<string> components;
  sfu::strings::split_into(&components,
      *path, {kPathSeparationChar, '\0'}, false);

  if (components.front() == "") {
    isAbsolute = true;
    components.erase(components.begin());
  }

  for (string component : components) {
    // Both 'xyz//abc' and 'zyx/./abc' means '/'. So skip.
    if (component == "" ||
        component == kPathSameDir) continue;
    if (component == kPathSubdir) {
      // Check if we traverse above root directory.
      if (out.empty()) return false;
      out.pop_back();
    }
    if (!IsValidFileName(component)) return false;

    out.push_back(component);
  }
  if(isAbsolute) {
    out.push_front("");
  }

  *path = sfu::strings::join(out, {kPathSeparationChar, '\0'});
  return true;
}

const std::string AbsolutePath(const std::string& path) {
  return AbsolutePath(path, CurrentWorkingDirectory());
}

const std::string AbsolutePath(const std::string& path,
                               const std::string& relative_to) {
  return "";
}

const std::string RelativePath(const std::string& absolute_path) {
  return RelativePath(absolute_path, CurrentWorkingDirectory());
}

const std::string RelativePath(const std::string& absolute_path,
                               const std::string& relative_from) {
  string absolute = absolute_path;
  ConsolidatePath(&absolute);

  string relative = relative_from;
  ConsolidatePath(&relative);

  if (absolute == relative) {
    return kPathSameDir;
  }

  list<string> absolute_components;
  sfu::strings::split_into(&absolute_components,
      absolute, {kPathSeparationChar, '\0'}, false);
  list<string> relative_components;
  sfu::strings::split_into(&relative_components,
      relative, {kPathSeparationChar, '\0'}, false);

  // Remove common path prefix.
  for (;;) {
    if (absolute_components.empty() || relative_components.empty()) {
      break;
    }
    if (absolute_components.front() != relative_components.front()) {
      break;
    }
    absolute_components.pop_front();
    relative_components.pop_front();
  }

  list<string> out;

  // Down-traverse from relative-to.
  if (!relative_components.empty()) {
    out.insert(out.begin(), relative_components.size(), kPathSubdir);
  }

  // Append the remaining absolute path.
  out.insert(out.end(), absolute_components.begin(), absolute_components.end());

  return sfu::strings::join(out, {kPathSeparationChar, '\0'});
}


}  // namespace sfu
