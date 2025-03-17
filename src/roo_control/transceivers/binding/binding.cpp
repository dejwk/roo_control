#include "roo_control/transceivers/binding/binding.h"

namespace roo_control {

roo_logging::Stream& operator<<(roo_logging::Stream& os,
                                const SensorBinding& binding) {
  os << binding.key_ << ":";
  if (binding.isBound()) {
    os << binding.get();
  } else {
    os << "<unbound>";
  }
  return os;
}

roo_logging::Stream& operator<<(roo_logging::Stream& os,
                                const BoundSensor& sensor) {
  os << sensor.binding_;
  return os;
}

}  // namespace roo_control