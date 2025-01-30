#pragma once

#include "roo_control/transceivers/id.h"

namespace roo_control {

// Stores (e.g. in Preferences) the mapping from binding keys to device IDs.
class SensorBindingStore {
 public:
  using Key = uint32_t;

  virtual UniversalTransceiverDeviceId getBinding(Key key) = 0;
  virtual void setBinding(Key key, UniversalTransceiverDeviceId id) = 0;
  virtual void clearBinding(Key key) = 0;
};

}  // namespace roo_control
