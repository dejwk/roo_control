#pragma once

#include <Arduino.h>

#include "roo_logging.h"

namespace roo_control {

// An abstraction of a multi-state switch. Can be used to represent GPIO relays,
// port extenders, or more sophisticated software switches.
//
// This abstraction serves two main purposes:
// 1. it makes it easy to swap the implementation of a switch (say, a relay)
//    without changing the code that uses it. For example, you can start by
//    connecting a relay to a GPIO port, but eventually upgrade to a port
//    extender. You will not need to update the calling code (just the setup
//    logic).
// 2. It allows building useful higher-level abstractions that are
//    implementation-independent. For example, the InertSwitch (and its
//    materialization, InertBinarySwitch) can be used to add debouncing on top
//    of any switch implementation.
template <typename State>
class Switch {
 public:
  virtual ~Switch() = default;

  // Returns the current state of the switch.
  //
  // If the implementation can fail to retrieve the state, the implementation
  // should LOG(ERROR) and return the last known state.
  virtual State getState() const = 0;

  // Updates the state of the switch. Returns true on success.
  virtual bool setState(State state) = 0;
};

enum BinarySwitchState { SWITCH_LOW = LOW, SWITCH_HIGH = HIGH };

// Materialization for a common case of a two-state switch, usually driven by
// digital logic.
using BinarySwitch = Switch<BinarySwitchState>;

// Basic logging support.
roo_logging::Stream& operator<<(roo_logging::Stream& out,
                                BinarySwitchState state);

}  // namespace roo_control