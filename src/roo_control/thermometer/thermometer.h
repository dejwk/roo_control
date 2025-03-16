#pragma once

#include "roo_logging.h"
#include "roo_temperature.h"

namespace roo_control {

// An abstraction of a termometer, i.e., a device that can report temperature.
class Thermometer {
 public:
  // Captures a temperature measured at a specific time.
  struct Reading {
    roo_temperature::Temperature value;
    roo_time::Uptime time;
  };

  virtual ~Thermometer() = default;

  // Returns the latest available temperature reading.
  virtual Reading readTemperature() const = 0;
};

roo_logging::Stream &operator<<(roo_logging::Stream &os,
                                const Thermometer::Reading &r);

}  // namespace roo_control