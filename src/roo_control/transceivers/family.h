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

  virtual TransceiverDeviceId deviceId(int idx) const = 0;

  virtual std::string deviceUserFriendlyName(TransceiverDeviceId id) const = 0;

  virtual Measurement read(TransceiverDeviceId id) const = 0;

  virtual void requestUpdate() = 0;

  virtual void addEventListener(TransceiverEventListener* listener) {}

  virtual void removeEventListener(TransceiverEventListener* listener) {}

 protected:
  TransceiverFamily() = default;
};

}  // namespace roo_control