#pragma once

#include "roo_control/binary_logical_state.h"
#include "roo_logging.h"

namespace roo_control {

/// An abstraction of a multi-state device.
///
/// This can represent a GPIO pushbutton, port extender, or software selector.
///
/// This abstraction serves two main purposes:
/// 1) it makes it easy to swap the implementation of a selector, without
///    changing the code that uses it. For example, you can start by
///    connecting a simple connector to a GPIO port, but eventually upgrade to a
///    port extender. You will not need to update the calling code (just the
///    setup logic).
/// 2) it allows building higher-level abstractions that are
///    implementation-independent. For example, the DebouncingSelector (and its
///    materialization, DebouncingBinarySelector) can be used to add debouncing
///    on top of any switch implementation.
template <typename State>
class Selector {
 public:
  virtual ~Selector() = default;

  /// Retrieves the current state, or returns false when it cannot be read.
  virtual bool getState(State& result) const = 0;
};

/// Materialization for a two-state selector, usually driven by digital logic.
using BinarySelector = Selector<BinaryLogicalState>;

}  // namespace roo_control