#pragma once

#include "roo_control/transceivers/binding/hal/store.h"
#include "roo_prefs.h"

namespace roo_control {

class ArduinoPreferencesTransceiverBindingStore
    : public TransceiverBindingStore {
 public:
  ArduinoPreferencesTransceiverBindingStore()
      : collection_("roo/ct/bindings") {}

  TransceiverSensorLocator getSensorBinding(SensorKey key) override;
  void setSensorBinding(SensorKey key,
                        const TransceiverSensorLocator& locator) override;
  void clearSensorBinding(SensorKey key) override;

 private:
  roo_prefs::Collection collection_;
};

}  // namespace roo_control
