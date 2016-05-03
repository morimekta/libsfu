#ifndef SFU_POPEN_H_
#define SFU_POPEN_H_

#include <string>
#include <vector>

namespace sfu {

class Popen {
  public:
    typedef enum {
      TIE = 0,
      PIPE
    } OutputMode;

    Popen(const std::vector<std::string>& cmd,
        OutputMode out_mode = TIE,
        OutputMode err_mode = TIE);
    ~Popen();

    inline int pid() { return pid_; }
    inline int pin() { return pin_; }
    inline int pout() { return pout_; }
    inline int perr() { return perr_; }

    int wait();

    // Convenience method that fills two strings with the output of stdout and
    // stderr from the process. Expects input-less commands, and will never
    // print out anything to console.
    static int run(const std::vector<std::string> cmd,
                   std::string *out = NULL,
                   std::string *err = NULL);

  private:
    int pid_;
    int status_;
    const std::vector<std::string>& cmd_;

    int pin_;   // pipe tied to 'stdin' in subprocess.
    int pout_;  // pipe tied to 'stdout'.
    int perr_;  // pipe tied to 'stderr'.
};

}  // namespace sfu

#endif  // SFU_POPEN_H_
