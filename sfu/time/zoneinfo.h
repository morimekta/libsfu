#ifndef SFU_TIME_ZONEINFO_H_
#define SFU_TIME_ZONEINFO_H_

#include <string>
#include <vector>

#include "sfu/time/time.h"

namespace sfu {
namespace time {

struct Zoneinfo_TZ;

// Simple Zoneinfo class that only cares for the "current" timezone.
class Zoneinfo {
  private:
    Zoneinfo() {}  // Disable default constructors.
    Zoneinfo(const Zoneinfo& zone) {}
    Zoneinfo(const std::string& locale);
    Zoneinfo(const std::string& zone, int16_t gmtoff);

  public:
    ~Zoneinfo();

    // Get the GMT offset at the given time, if timestamp is not given, use
    // "now".
    time_t utcoffset(time_t utc = kNowTimestamp) const;
    // Give the Normal or DST timezone offset for the given valid at the UTC
    // timestamp. If no timestamp use "now".
    time_t localtime(time_t utc = kNowTimestamp) const;
    // The name of the timezone valid at given timestamp. If no timestamp use
    // "now".
    const std::string& zone(time_t utc = kNowTimestamp) const;

    static const Zoneinfo* GetZoneinfo(const std::string& name);
  private:
    // Locale name for the timezone. E.g. "Europe/Zurich"
    std::string locale_;

    // The timezone info database.
    std::vector<Zoneinfo_TZ*> db_;

    // ---- methods ----
    void LoadInfo();
    const Zoneinfo_TZ* ZoneForTimestamp(time_t ts) const;
};


};  // namespace time
};  // namespace sfu


#endif  // SFU_TIME_ZONEINFO_H_
