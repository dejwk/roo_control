#pragma once

#include "roo_control/binary_logical_state.h"
#include "roo_control/selector.h"
#include "roo_logging.h"

namespace roo_control {

// An abstraction of a multi-state settable switch. Can be used to represent
// GPIO relays, port extenders, or more sophisticated software switches.
//
// This abstraction serves two main purposes:
// 1. it makes it easy to swap the implementation of a switch, without
// changing the code that uses it. For example, you can start by
//    connecting a simple relay to a GPIO port, but eventually upgrade to a
//    port extender. You will not need to update the calling code (just the
//    setup logic).
// 2. It allows building useful higher-level abstractions that are
//    implementation-independent. For example, the InertSwitch (and its
//    materialization, InertBinarySwitch) can be used to add debouncing
//    on top of any switch implementation.
template <typename State>
class Switch : public Selector<State> {
 public:
  virtual ~Switch() = default;

  // Updates the state of the switch. Returns true on success.
  virtual bool setState(State state) = 0;
};

// Materialization for a common case of a two-state switch, usually driven by
// digital logic.
using BinarySwitch = Switch<BinaryLogicalState>;

}  // namespace roo_control