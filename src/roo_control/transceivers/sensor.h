#pragma once

#include "roo_control/transceivers/id.h"
#include "roo_temperature.h"
#include "roo_control/transceivers/measurement.h"

namespace roo_control {

class TransceiverEventListener {
 public:
  virtual ~TransceiverEventListener() = default;

  virtual void devicesChanged() {}
  virtual void newReadingsAvailable() {}
};

class Sensor {
 public:
  virtual Measurement read() = 0;
//  public:
//   virtual SensorDeviceId id() const = 0;
};

class Actuator {
 public:
  virtual void write(float val) = 0;
};

}  // namespace roo_control