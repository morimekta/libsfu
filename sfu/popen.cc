#include <sys/wait.h>
#include <unistd.h>

#include "sfu/popen.h"

using namespace std;

namespace sfu {
namespace popen_internal {

void close_pipefd(int p) {
  if (p > 0) close(p);
}

void close_pipe(int p[2]) {
  close_pipefd(p[0]);
  close_pipefd(p[1]);
}

void replace_fd(int fileno, int pipeno) {
  close(fileno);
  dup(pipeno);
}

}  // namespace popen_internal

Popen::Popen(const vector<string>& cmd,
    OutputMode out_mode, OutputMode err_mode) : pid_(0), status_(0), cmd_(cmd),
    pin_(-1), pout_(-1), perr_(-1) {
  int pin_a[2] = {-1, -1};
  int pout_a[2] = {-1, -1};
  int perr_a[2] = {-1, -1};

  if (pipe(pin_a) < 0) {
    return;
  }

  if (out_mode == Popen::PIPE && pipe(pout_a) < 0) {
    popen_internal::close_pipe(pin_a);
    return;
  }

  if (err_mode == Popen::PIPE && pipe(perr_a) < 0) {
    popen_internal::close_pipe(pin_a);
    popen_internal::close_pipe(pout_a);
    return;
  }

  pid_ = fork();
  if (pid_ > 0) {  // Parent process
    popen_internal::close_pipefd(pin_a[0]);
    popen_internal::close_pipefd(pout_a[1]);
    popen_internal::close_pipefd(perr_a[1]);
    pin_  = pin_a[1];
    pout_ = pout_a[0];
    perr_ = perr_a[0];
  } else if (pid_ == 0) {  // Child process
    popen_internal::close_pipefd(pin_a[1]);
    popen_internal::replace_fd(0, pin_a[0]);

    if (out_mode == Popen::PIPE) {
      popen_internal::close_pipefd(pout_a[0]);
      popen_internal::replace_fd(1, pout_a[1]);
    }

    if (err_mode == Popen::PIPE) {
      popen_internal::close_pipefd(perr_a[0]);
      popen_internal::replace_fd(2, perr_a[1]);
    }

    // parse cmd to char** argv.
    const char **argv = static_cast<const char**>(
        alloca((cmd_.size() + 1) * sizeof(char*)));
    argv[cmd_.size()] = NULL;
    for (size_t i = 0; i < cmd_.size(); ++i) {
      argv[i] = cmd_[i].c_str();
    }

    // parse cmd to char** argv.
    execvp(cmd_[0].c_str(), const_cast<char**>(argv));
    exit(1);  // If execvp fails.
  } else {
    // Fork error.
    popen_internal::close_pipe(pin_a);
    popen_internal::close_pipe(pout_a);
    popen_internal::close_pipe(perr_a);
    pid_ = -1;
  }
}

Popen::~Popen() {
  wait();
}

int Popen::wait() {
  if (pid_ == 0) return status_;
  if (pid_ < 0) return -1;

  popen_internal::close_pipefd(pin_);
  popen_internal::close_pipefd(pout_);
  popen_internal::close_pipefd(perr_);

  pin_ = pout_ = perr_ = 0;

  if (waitpid(pid_, &status_, 0) < 0) {
    // something failed???
    pid_ = 0;
    return -1;
  }
  pid_ = 0;
  return status_;
}

int Popen::run(const std::vector<std::string> cmd,
               std::string *out,
               std::string *err) {
  char buffer[256] = {0, };
  size_t p = 0;

  Popen popen(cmd, Popen::PIPE, Popen::PIPE);
  if (popen.pid() <= 0) {
    return -1;
  }

  fd_set fs;
  bool outend = false;
  bool errend = false;

  FD_ZERO(&fs);
  FD_SET(popen.pout(), &fs);
  FD_SET(popen.perr(), &fs);
  int max_fd = std::max(popen.pout(), popen.perr());

  // Reading the output buffer, even if no output is needed from the command
  // may ensure that it completes instead of running out of write buffer, and
  // halting that way.
  while (select(max_fd + 1, &fs, NULL, NULL, NULL) > 0) {
    if (FD_ISSET(popen.pout(), &fs)) {
      p = read(popen.pout(), buffer, 255);
      if (p <= 0) {
        FD_CLR(popen.pout(), &fs);
        outend = true;
      } else if (out != NULL) {
        out->append(buffer, p);
      }
    }
    if (FD_ISSET(popen.perr(), &fs)) {
      p = read(popen.perr(), buffer, 255);
      if (p <= 0) {
        FD_CLR(popen.perr(), &fs);
        errend = true;
      } else if (err != NULL) {
        err->append(buffer, p);
      }
    }

    if (outend && errend) break;

    if (!outend) FD_SET(popen.pout(), &fs);
    if (!errend) FD_SET(popen.perr(), &fs);
  }

  return popen.wait();
}

}  // namespace sfu
