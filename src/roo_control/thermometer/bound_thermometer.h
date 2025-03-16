#pragma once

#include "roo_control/thermometer/thermometer.h"
#include "roo_control/transceivers/binding/binding.h"

namespace roo_control {

class BoundThermometer : public Thermometer {
 public:
  BoundThermometer(TransceiverUniverse& universe, const SensorBinding& binding)
      : bound_sensor_(universe, binding) {}

  Reading readTemperature() const override {
    Measurement m = bound_sensor_.read();
    if (m.isDefined()) {
      CHECK_EQ(roo_control_Quantity_kTemperature, m.quantity());
    }
    return Reading{.value = roo_temperature::DegCelcius(m.value()),
                   .time = m.time()};
  }

 private:
  BoundSensor bound_sensor_;
};

}
