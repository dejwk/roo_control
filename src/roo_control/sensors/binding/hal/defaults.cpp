#include "roo_control/sensors/binding/hal/defaults.h"

namespace roo_control {

SensorBindingStore& DefaultSensorBindingStore() {
  static ArduinoPreferencesSensorBindingStore store;
  return store;
}

}