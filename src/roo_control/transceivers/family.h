#pragma once

#include <string>

#include "roo_collections.h"
#include "roo_collections/flat_small_hash_set.h"
#include "roo_control/transceivers/id.h"
#include "roo_control/transceivers/measurement.h"
#include "roo_control/transceivers/sensor.h"

namespace roo_control {

class TransceiverFamily {
 public:
  virtual ~TransceiverFamily() = default;

  virtual int deviceCount() const = 0;

  virtual TransceiverDeviceId deviceId(size_t device_idx) const = 0;

  virtual std::string deviceUserFriendlyName(
      TransceiverDeviceId device_id) const = 0;

  virtual size_t getSensorCount(TransceiverDeviceId device_id) const = 0;

  virtual std::string sensorUserFriendlyName(TransceiverDeviceId device_id,
                                             SensorIdx sensor_idx) const = 0;

  virtual roo_control_Quantity getSensorQuantity(
      TransceiverDeviceId device_id, SensorIdx sensor_idx) const = 0;

  virtual Measurement read(TransceiverDeviceId device_id,
                           SensorIdx sensor_idx) const = 0;

  virtual size_t getActuatorCount(TransceiverDeviceId device_id) const = 0;

  virtual std::string actuatorUserFriendlyName(
      TransceiverDeviceId device_id, ActuatorIdx actuator_idx) const = 0;

  virtual roo_control_Quantity getActuatorQuantity(
      TransceiverDeviceId device_id, ActuatorIdx actuator_idx) const = 0;

  virtual void write(TransceiverDeviceId device_id, ActuatorIdx actuator_idx,
                     float value) const = 0;

  virtual void requestUpdate() = 0;

  virtual void addEventListener(TransceiverEventListener* listener) {}

  virtual void removeEventListener(TransceiverEventListener* listener) {}

 protected:
  TransceiverFamily() = default;
};

// For devices that have a single sensor (and no actuators).
class SimpleSensorFamily : public TransceiverFamily {
 public:
  size_t getSensorCount(TransceiverDeviceId device_id) const override {
    return 1;
  }

  std::string sensorUserFriendlyName(TransceiverDeviceId device_id,
                                     SensorIdx sensor_idx) const override {
    CHECK_EQ(0, sensor_idx);
    return deviceUserFriendlyName(device_id);
  }

  roo_control_Quantity getSensorQuantity(TransceiverDeviceId device_id,
                                         SensorIdx sensor_idx) const override {
    CHECK_EQ(0, sensor_idx);
    return getSensorQuantity(device_id);
  }

  Measurement read(TransceiverDeviceId device_id,
                   SensorIdx sensor_idx) const override {
    CHECK_EQ(0, sensor_idx);
    return readSensor(device_id);
  }

  size_t getActuatorCount(TransceiverDeviceId device_id) const override {
    return 0;
  }

  std::string actuatorUserFriendlyName(
      TransceiverDeviceId device_id, ActuatorIdx actuator_idx) const override {
    LOG(FATAL) << "This device has no actuators.";
    return "";
  }

  roo_control_Quantity getActuatorQuantity(
      TransceiverDeviceId device_id, ActuatorIdx actuator_idx) const override {
    LOG(FATAL) << "This device has no actuators.";
    return roo_control_Quantity_kUnspecified;
  }

  void write(TransceiverDeviceId device_id, ActuatorIdx actuator_idx,
             float value) const override {
    LOG(FATAL) << "This device has no actuators.";
  }

 protected:
  virtual Measurement readSensor(TransceiverDeviceId device_id) const = 0;

  virtual roo_control_Quantity getSensorQuantity(
      TransceiverDeviceId device_id) const = 0;
};

}  // namespace roo_control