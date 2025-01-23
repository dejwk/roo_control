#pragma once

#include <string>

#include "roo_collections.h"
#include "roo_collections/flat_small_hash_set.h"
#include "roo_control/sensors/device_id.h"
#include "roo_control/sensors/measurement.h"
#include "roo_control/sensors/sensor.h"

namespace roo_control {

class SensorFamily {
 public:
  virtual ~SensorFamily() = default;

  virtual int sensorCount() const = 0;

  virtual SensorDeviceId sensorId(int idx) const = 0;

  virtual std::string sensorUserFriendlyName(SensorDeviceId id) const = 0;

  virtual Measurement read(SensorDeviceId id) const = 0;

  virtual void requestUpdate() = 0;

  virtual void addEventListener(SensorEventListener* listener) {}

  virtual void removeEventListener(SensorEventListener* listener) {}

 protected:
  SensorFamily() = default;
};

}  // namespace roo_control