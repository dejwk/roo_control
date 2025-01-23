#pragma once

#include "roo_logging.h"

namespace roo_control {

enum BinaryLogicalState { BINARY_STATE_LOW = 0, BINARY_STATE_HIGH = 1 };

// Basic logging support.
roo_logging::Stream& operator<<(roo_logging::Stream& out,
                                BinaryLogicalState state);

}
