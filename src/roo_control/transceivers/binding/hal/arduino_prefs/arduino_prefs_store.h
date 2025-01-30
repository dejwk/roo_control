#pragma once

#include "roo_control/transceivers/binding/hal/store.h"
#include "roo_prefs.h"

namespace roo_control {

class ArduinoPreferencesSensorBindingStore : public SensorBindingStore {
 public:
  ArduinoPreferencesSensorBindingStore() : collection_("roo/1w/bindings") {}

  UniversalSensorId getBinding(Key key) override;
  void setBinding(Key key, UniversalSensorId id) override;
  void clearBinding(Key key) override;

 private:
  roo_prefs::Collection collection_;
};

}  // namespace roo_control
