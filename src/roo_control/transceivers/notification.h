#pragma once

#include "roo_control/transceivers/id.h"
#include "roo_control/transceivers/measurement.h"

namespace roo_control {

class TransceiverEventListener {
 public:
  virtual ~TransceiverEventListener() = default;

  virtual void devicesChanged() {}
  virtual void newReadingsAvailable() {}
};

}  // namespace roo_control