#ifndef SFU_TIME_TIME_H_
#define SFU_TIME_TIME_H_

namespace sfu {
namespace time {

// Somewhere around 1830, well before time was standardized.
static const time_t kNowTimestamp = -4417984800;

// RFC-822 (updated in RFC-1123) is the original date format for emails, making
// it a de-facto standard for internet passed date formats. Example:
// "May 31 22:46:50 2014" (skipping day-of-week and timezone).
const char* kRfc822DateFormat = "%b %d %H:%M:%S %Y";

// ISO-8603 or RFC-3339 is the standard time format on the Internet. Well, in
// theory, as RFC-822 is still the main format used in Emails and HTTP
// messages. Example
// 2014-05-31 22:46:50 (skipping timezone).
const char* kIso8601DateFormat = "%Y-%m-%d %H:%M:%S";

// Get the current UTC timestamp.
time_t now();

};  // namespace time
};  // namespace sfu

#endif  // SFU_TIME_TIME_H_
