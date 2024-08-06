#include "roo_control/switch.h"

namespace roo_control {

roo_logging::Stream& operator<<(roo_logging::Stream& out,
                                BinarySwitchState state) {
  switch (state) {
    case SWITCH_LOW: {
      out << "LOW";
      break;
    }
    case SWITCH_HIGH: {
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