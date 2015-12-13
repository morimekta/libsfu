#ifndef UTIL_TIME_DATETIME_H_
#define UTIL_TIME_DATETIME_H_

#include <string>
#include <memory>
#include <vector>

#include "sfu/time/time.h"

namespace sfu {
namespace time {

class Zoneinfo;


class Datetime {
  public:
    // Default Datetime is initialized with the current time and local timezone.
    Datetime();
    // Copy constructor
    Datetime(const Datetime& dt);
    // Move constructor
    Datetime(Datetime&& dt);
    ~Datetime();

    // The unix timestamp.
    time_t unix();
    time_t localtime();

    // The year according to the Gregorian calendar.
    int32_t year() const;
    // The Gregorian calendar month number, 1-12.
    uint8_t month() const;
    // The day of the month 1-31
    uint8_t day() const;
    // The hour of day (according to local time). 0-24
    uint8_t hour() const;
    // The minute of the hour. 0-59
    uint8_t minute() const;
    // The second of the minute. 0-60 (to accommodate for leap seconds).
    uint8_t second() const;

    // The hour using clock counting: 1-12.
    uint8_t clockHour() const;
    // Whether the clock is on the PM round, If false is AM.
    bool    clockIsPm() const;

    // Make a readable string from the
    const std::string toString() const;
    const std::string toISOString() const;
    const std::string format(const char* format) const;

  private:
    // UTC timestamp.
    time_t timestamp_;
    // Timezone 
    const Zoneinfo* timezone_;
};

};  // namespace time
};  // namespace sfu


#endif  // UTIL_TIME_DATETIME_H_
