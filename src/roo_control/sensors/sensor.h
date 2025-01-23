#pragma once

#include "roo_control/sensors/device_id.h"
#include "roo_temperature.h"
#include "roo_control/sensors/measurement.h"

namespace roo_control {

class SensorEventListener {
 public:
  virtual ~SensorEventListener() = default;

  virtual void sensorsChanged() {}
  virtual void newReadingsAvailable() {}
};

class Sensor {
 public:
  virtual Measurement read() = 0;
//  public:
//   virtual SensorDeviceId id() const = 0;
};

}  // namespace roo_control