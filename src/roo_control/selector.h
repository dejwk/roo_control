#pragma once

#include "roo_logging.h"
#include "roo_control/binary_logical_state.h"

namespace roo_control {

// An abstraction of a multi-state device, such as a GPIO pushbutton, or
// something more sophisticated like a port extender or software selector.
//
// This abstraction serves two main purposes:
// 1. it makes it easy to swap the implementation of a selector, without
// changing the code that uses it. For example, you can start by
//    connecting a simple connector to a GPIO port, but eventually upgrade to a
//    port extender. You will not need to update the calling code (just the
//    setup logic).
// 2. It allows building useful higher-level abstractions that are
//    implementation-independent. For example, the DebouncingSelector (and its
//    materialization, DebouncingBinarySelector) can be used to add debouncing
//    on top of any switch implementation.
template <typename State>
class Selector {
 public:
  virtual ~Selector() = default;

  // Returns the current state of the switch.
  //
  // If the implementation can fail to retrieve the state, the implementation
  // should LOG(ERROR) and return the last known state.
  virtual State getState() const = 0;
};

// Materialization for a common case of a two-state selector, usually driven by
// digital logic.
using BinarySelector = Selector<BinaryLogicalState>;

}  // namespace roo_control