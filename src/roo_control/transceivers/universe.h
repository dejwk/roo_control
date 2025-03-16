#pragma once

// #include <vector>

#include "roo_collections/flat_small_hash_map.h"
#include "roo_control/transceivers/id.h"
#include "roo_control/transceivers/notification.h"
#include "roo_logging.h"

namespace roo_control {

class TransceiverUniverse {
 public:
  virtual ~TransceiverUniverse() = default;

  virtual int deviceCount() const = 0;

  virtual void forEachDevice(
      std::function<bool(const TransceiverDeviceLocator&)> callback) const = 0;

  virtual bool getDeviceDescriptor(
      const TransceiverDeviceLocator& locator,
      roo_control_TransceiverDescriptor& descriptor) const = 0;

  virtual Measurement read(const TransceiverSensorLocator& locator) const = 0;

  virtual bool write(const TransceiverActuatorLocator& locator,
                     float value) const = 0;

  virtual void requestUpdate() = 0;

  virtual void addEventListener(TransceiverEventListener* listener) {}

  virtual void removeEventListener(TransceiverEventListener* listener) {}
};

class SimpleSensorUniverse : public TransceiverUniverse {
 public:
  bool getDeviceDescriptor(
      const TransceiverDeviceLocator& locator,
      roo_control_TransceiverDescriptor& descriptor) const override {
    descriptor.sensors_count = 1;
    descriptor.sensors[0].id[0] = 0;
    descriptor.sensors[0].quantity = getSensorQuantity(locator);
    descriptor.actuators_count = 0;
    return true;
  }

  Measurement read(const TransceiverSensorLocator& locator) const override {
    CHECK(locator.sensor_id().empty());
    return readSensor(locator.device_locator());
  }

  bool write(const TransceiverActuatorLocator& locator,
             float value) const override {
    LOG(FATAL) << "This device has no actuators.";
    return false;
  }

 protected:
  virtual Measurement readSensor(
      const TransceiverDeviceLocator& locator) const = 0;

  virtual roo_control_Quantity getSensorQuantity(
      TransceiverDeviceLocator device_locator) const = 0;
};

}  // namespace roo_control