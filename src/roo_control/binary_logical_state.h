#pragma once

#include "roo_logging.h"

namespace roo_control {

/// Binary logical state used by selectors and switches.
enum BinaryLogicalState { BINARY_STATE_LOW = 0, BINARY_STATE_HIGH = 1 };

/// Streams a human-readable state name.
roo_logging::Stream& operator<<(roo_logging::Stream& out,
                                BinaryLogicalState state);

}  // namespace roo_control
