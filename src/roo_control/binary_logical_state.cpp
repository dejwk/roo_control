#include "roo_control/binary_logical_state.h"

namespace roo_control {

roo_logging::Stream& operator<<(roo_logging::Stream& out,
                                BinaryLogicalState state) {
  switch (state) {
    case BINARY_STATE_LOW: {
      out << "LOW";
      break;
    }
    case BINARY_STATE_HIGH: {
      out << "HIGH";
      break;
    }
    default: {
      out << "<INVALID>";
      break;
    }
  }
  return out;
}

}  // namespace roo_control