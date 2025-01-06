#pragma once

#include "roo_control/sensors/binding/hal/store.h"
#include "roo_prefs.h"

namespace roo_control {

class ArduinoPreferencesSensorBindingStore : public SensorBindingStore {
 public:
  ArduinoPreferencesSensorBindingStore() : collection_("roo/1w/bindings") {}

  UniversalDeviceId getBinding(Key key) override;
  void setBinding(Key key, UniversalDeviceId id) override;
  void clearBinding(Key key) override;

 private:
  roo_prefs::Collection collection_;
};

}  // namespace roo_control
