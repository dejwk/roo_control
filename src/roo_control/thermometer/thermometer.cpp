#include "roo_control/thermometer/thermometer.h"

namespace roo_control {

roo_logging::Stream& operator<<(roo_logging::Stream& os,
                                const Thermometer::Reading& r) {
  os << r.value << " measuread at " << r.time << " ("
     << (roo_time::Uptime::Now() - r.time) << "s ago)";
  return os;
}

}
