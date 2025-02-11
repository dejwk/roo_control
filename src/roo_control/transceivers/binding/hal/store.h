#pragma once

#include "roo_control/transceivers/id.h"

namespace roo_control {

// Stores (e.g. in Preferences) the mapping from binding keys to device IDs.
class TransceiverBindingStore {
 public:
  using SensorKey = uint32_t;

  virtual TransceiverSensorLocator getSensorBinding(SensorKey key) = 0;
  virtual void setSensorBinding(SensorKey key,
                                const TransceiverSensorLocator& locator) = 0;
  virtual void clearSensorBinding(SensorKey key) = 0;
};

}  // namespace roo_control
