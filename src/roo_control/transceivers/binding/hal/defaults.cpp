#include "roo_control/transceivers/binding/hal/defaults.h"

namespace roo_control {

SensorBindingStore& DefaultSensorBindingStore() {
  static ArduinoPreferencesSensorBindingStore store;
  return store;
}

}