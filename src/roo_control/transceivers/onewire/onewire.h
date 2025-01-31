#pragma once

#include "roo_control/transceivers/family.h"
#include "roo_control/transceivers/id.h"
#include "roo_io/text/string_printf.h"
#include "roo_onewire.h"
#include "roo_temperature.h"

namespace roo_control {

class OneWireFamily : public SimpleSensorFamily {
 public:
  OneWireFamily(roo_onewire::OneWire& onewire)
      : onewire_(onewire), listener_(*this) {
    onewire_.thermometers().addEventListener(&listener_);
  }

  ~OneWireFamily() { onewire_.thermometers().removeEventListener(&listener_); }

  int deviceCount() const override { return onewire_.thermometers().count(); }

  TransceiverDeviceId deviceId(size_t idx) const override {
    return (TransceiverDeviceId)onewire_.thermometers().rom_code(idx).raw();
  }

  std::string deviceUserFriendlyName(
      TransceiverDeviceId device_id) const override {
    char code[17];
    roo_onewire::RomCode(device_id).toCharArray(code);
    code[16] = 0;
    return std::string("1-Wire:") + code;
  }

  roo_control_Quantity getSensorQuantity(
      TransceiverDeviceId device_id) const override {
    return roo_control_Quantity_kTemperature;
  }

  Measurement readSensor(TransceiverDeviceId device_id) const override {
    const roo_onewire::Thermometer* t =
        onewire_.thermometers().thermometerByRomCode(
            roo_onewire::RomCode(device_id));
    return Measurement(roo_control_Quantity_kTemperature,
                       roo_time::Uptime::Now(),
                       t == nullptr ? nanf("") : t->temperature().degCelcius());
  }

  void requestUpdate() override { onewire_.update(); }

  void addEventListener(TransceiverEventListener* listener) override {
    auto result = event_listeners_.insert(listener);
    CHECK(result.second) << "Event listener " << listener
                         << " was registered already.";
  }

  void removeEventListener(TransceiverEventListener* listener) override {
    event_listeners_.erase(listener);
  }

 private:
  void notifySensorsChanged() const {
    for (auto& listener : event_listeners_) {
      listener->devicesChanged();
    }
  }

  void notifyNewReadingsAvailable() const {
    for (auto& listener : event_listeners_) {
      listener->newReadingsAvailable();
    }
  }

  class Listener : public roo_onewire::Thermometers::EventListener {
   public:
    Listener(OneWireFamily& onewire) : onewire_(onewire) {}

    void discoveryCompleted() const override {
      onewire_.notifySensorsChanged();
    }

    void conversionCompleted() const override {
      onewire_.notifyNewReadingsAvailable();
    }

   private:
    OneWireFamily& onewire_;
  };

  roo_collections::FlatSmallHashSet<TransceiverEventListener*> event_listeners_;
  roo_onewire::OneWire& onewire_;
  Listener listener_;
};

}  // namespace roo_control