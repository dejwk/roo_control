#pragma once

#include "roo_control/sensors/device_id.h"
#include "roo_control/sensors/family.h"
#include "roo_io/text/string_printf.h"
#include "roo_onewire.h"
#include "roo_temperature.h"

namespace roo_control {

class OneWireFamily : public SensorFamily {
 public:
  OneWireFamily(roo_onewire::OneWire& onewire)
      : onewire_(onewire), listener_(*this) {
    onewire_.thermometers().addEventListener(&listener_);
  }

  ~OneWireFamily() { onewire_.thermometers().removeEventListener(&listener_); }

  int sensorCount() const override { return onewire_.thermometers().count(); }

  SensorDeviceId sensorId(int idx) const override {
    return (SensorDeviceId)onewire_.thermometers().rom_code(idx).raw();
  }

  std::string sensorUserFriendlyName(SensorDeviceId id) const override {
    char code[17];
    roo_onewire::RomCode(id).toCharArray(code);
    code[16] = 0;
    return std::string("1-Wire:") + code;
  }

  roo_temperature::Thermometer::Reading readTemperature(
      SensorDeviceId id) const override {
    const roo_onewire::Thermometer* t =
        onewire_.thermometers().thermometerByRomCode(roo_onewire::RomCode(id));
    if (t == nullptr) {
      return roo_temperature::Thermometer::Reading{
          .value = roo_temperature::Temperature(),
          .time = roo_time::Uptime::Now(),
      };
    }
    return roo_temperature::Thermometer::Reading{
        .value = t->temperature(),
        .time = roo_time::Uptime::Now(),
    };
  }

  void requestUpdate() override {
    onewire_.update();
  }

  void addEventListener(SensorEventListener* listener) override {
    auto result = event_listeners_.insert(listener);
    CHECK(result.second) << "Event listener " << listener
                         << " was registered already.";
  }

  void removeEventListener(SensorEventListener* listener) override {
    event_listeners_.erase(listener);
  }

 private:
  void notifySensorsChanged() const {
    for (auto& listener : event_listeners_) {
      listener->sensorsChanged();
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

  roo_collections::FlatSmallHashSet<SensorEventListener*> event_listeners_;
  roo_onewire::OneWire& onewire_;
  Listener listener_;
};

}  // namespace roo_control