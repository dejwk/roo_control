#include "roo_control/switch/inert_switch.h"

namespace roo_control {

roo_time::Duration DefaultBackoff(int retry_count) {
  float min_delay_us = 1000.0f;     // 1 ms
  float max_delay_us = 5000000.0f;  // 5 s
  float delay = pow(1.33, retry_count) * min_delay_us;
  if (delay > max_delay_us) {
    delay = max_delay_us;
  }
  // Randomize by +=20%, to make unrelated retries spread more evenly in time.
  delay += (float)delay * ((float)rand() / RAND_MAX - 0.5f) * 0.4f;
  return roo_time::Micros((uint64_t)delay);
}

}  // namespace roo_control
