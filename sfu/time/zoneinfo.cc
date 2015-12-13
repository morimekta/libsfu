#include "sfu/time/zoneinfo.h"

#include "sfu/numbers.h"
#include "sfu/popen.h"
#include "sfu/strings/strings.h"

#include <regex>
#include <ctime>

using namespace std;

namespace sfu {
namespace time {

struct Zoneinfo_TZ {
  // Timezone name (not locale name).
  string name;
  // True if this is a daylight saving time.
  bool is_dst;
  // UTC offset in seconds east of GMT.
  int16_t gmtoff;
  // The UTC timestamp it takes effect.
  time_t start;
};

Zoneinfo::Zoneinfo(const std::string& locale) : locale_(locale) {
  LoadInfo();
}

Zoneinfo::Zoneinfo(const string& zone, int16_t gmtoff) : locale_(zone) {
  db_.push_back(new Zoneinfo_TZ);
  db_[0]->name = zone;
  db_[0]->start = kNowTimestamp;
  db_[0]->gmtoff = gmtoff;
  // TODO(steineldar): Look up the zone from a list of known DST names.
  db_[0]->is_dst = false;
}

Zoneinfo::~Zoneinfo() {
  for (Zoneinfo_TZ* i : db_) {
    delete i;
  }
}

// public methods

time_t Zoneinfo::utcoffset(time_t utc) const {
  if (utc == kNowTimestamp) {
    utc = now();
  }

  const Zoneinfo_TZ *info = ZoneForTimestamp(utc);
  // For UTC time.
  if (!info) return 0;
  else return info->gmtoff;
}

time_t Zoneinfo::localtime(time_t utc) const {
  if (utc == kNowTimestamp) {
    utc = now();
  }

  const Zoneinfo_TZ *info = ZoneForTimestamp(utc);
  // For UTC time.
  if (!info) return utc;
  else return utc + info->gmtoff;
}

// static

const Zoneinfo* Zoneinfo::GetZoneinfo(const std::string& name) {
  return NULL;
}

// private

void Zoneinfo::LoadInfo() {
  vector<string> zdump;
  zdump.push_back("zdump");
  zdump.push_back("-v");
  zdump.push_back(locale_);

  string out, err;
  int ret = Popen::run(zdump, &out, &err);
  if (ret == 0 && err.empty()) {
    list<string> lines;
    sfu::strings::split_into(&lines, out, "\n", true);

    regex re("^"
        "[a-zA-Z/]+  "
        "\\w{3} (\\w{3} \\d\\d \\d\\d:\\d\\d:\\d\\d \\d{4}) UTC" // date
        " = "
        "\\w{3} \\w{3} \\d\\d \\d\\d:\\d\\d:\\d\\d  \\d{4} "
        "(\\w*) "        // zone
        "isdst=(\\d) "   // isdst
        "gmtoff=(\\d*)"  // gmtoff
        "$");

    vector<Zoneinfo_TZ*> add;

    for (string line : lines) {
      smatch res;
      regex_match(line, res, re);
      if (res.size() == 5) {
        string date_s  (res[1].first, res[1].second);
        string zone    (res[2].first, res[2].second);
        string isdst_s (res[3].first, res[3].second);
        string gmtoff_s(res[4].first, res[4].second);

        bool isdst;
        int16_t gmtoff;
        struct tm t;

        if (!safe_str2bool(isdst_s, &isdst)) continue;
        if (!safe_str2int16(gmtoff_s, &gmtoff)) continue;
        if (!strptime(date_s.c_str(), kRfc822DateFormat, &t)) continue;
        // The 59 second is always preceeded one second later with a new
        // timezone. LMT is the pre-standardized time offset, and these usually
        // have odd minute and second values.
        if (t.tm_sec == 59 && zone != "LMT") continue;

        time_t ts = mktime(&t);

        Zoneinfo_TZ *tz = new Zoneinfo_TZ;
        tz->name = zone;
        tz->is_dst = isdst;
        tz->gmtoff = gmtoff;
        tz->start = ts;

        db_.push_back(tz);
      }
    }
  }
}

const Zoneinfo_TZ* Zoneinfo::ZoneForTimestamp(time_t ts) const {
  if (db_.empty()) {
    return NULL;
  }

  auto i = db_.begin();
  const Zoneinfo_TZ* last = *i;
  for (; i != db_.end(); ++i) {
    if ((*i)->start > ts) {
      return last;
    }
    last = *i;
  }
  return last;
}

};  // namespace time
};  // namespace sfu
