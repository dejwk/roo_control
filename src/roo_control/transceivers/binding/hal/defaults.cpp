#include "roo_control/transceivers/binding/hal/defaults.h"

namespace roo_control {

TransceiverBindingStore& DefaultTransceiverBindingStore() {
  static ArduinoPreferencesTransceiverBindingStore store;
  return store;
}

}