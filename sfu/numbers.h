#ifndef SFU_NUMBERS_H_
#define SFU_NUMBERS_H_

#include <cstdint>
#include "sfu/strings/cord.h"

namespace sfu {

bool safe_str2bool(const strings::cord& str, bool* value);

bool safe_str2int8(const strings::cord& str, int8_t *num, int base = 10);
bool safe_str2uint8(const strings::cord& str, uint8_t *num, int base = 10);

bool safe_str2int16(const strings::cord& str, int16_t *num, int base = 10);
bool safe_str2uint16(const strings::cord& str, uint16_t *num, int base = 10);

bool safe_str2int32(const strings::cord& str, int32_t *num, int base = 10);
bool safe_str2uint32(const strings::cord& str, uint32_t *num, int base = 10);

bool safe_str2int64(const strings::cord& str, int64_t *num, int base = 10);
bool safe_str2uint64(const strings::cord& str, uint64_t *num, int base = 10);

bool safe_str2f(const strings::cord& str, float* dbl);
bool safe_str2d(const strings::cord& str, double* dbl);
bool safe_str2ld(const strings::cord& str, long double* dbl);

}  // namespace sfu

#endif  // SFU_NUMBERS_H_
