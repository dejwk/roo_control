#pragma once

#include "roo_logging.h"
#include "roo_quantity.h"
#include "roo_quantity/temperature.h"

namespace roo_control {

/// An abstraction of a thermometer, i.e., a device that reports temperature.
class Thermometer {
 public:
  /// Captures a temperature measured at a specific time.
  struct Reading {
    /// Measured temperature value.
    roo_quantity::Temperature value;
    /// Timestamp of the reading.
    roo_time::Uptime time;
  };

  virtual ~Thermometer() = default;

  /// Returns the latest available temperature reading.
  virtual Reading readTemperature() const = 0;
};

/// Streams a human-readable reading.
roo_logging::Stream &operator<<(roo_logging::Stream &os,
                                const Thermometer::Reading &r);

}  // namespace roo_control