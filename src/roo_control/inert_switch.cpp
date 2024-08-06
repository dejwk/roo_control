#include "roo_control/inert_switch.h"

namespace roo_control {

roo_time::Interval DefaultBackoff(int retry_count) {
  uint64_t min_delay_us = 200;      // 0.2 ms
  uint64_t max_delay_us = 1000000;  // 1s
  float delay = pow(1.33, retry_count) * min_delay_us;
  if (delay > max_delay_us) {
    return roo_time::Micros(max_delay_us);
  }
  return roo_time::Micros((uint64_t)delay);
}

}  // namespace roo_control
