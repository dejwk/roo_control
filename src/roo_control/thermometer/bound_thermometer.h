#pragma once

#include "roo_control/thermometer/thermometer.h"
#include "roo_transceivers/binding/binding.h"

namespace roo_control {

class BoundThermometer : public Thermometer {
 public:
  BoundThermometer(roo_transceivers::Universe& universe,
                   const roo_transceivers::SensorBinding* binding)
      : bound_sensor_(universe, binding) {}

  Reading readTemperature() const override {
    roo_transceivers::Measurement m = bound_sensor_.read();
    if (m.isDefined()) {
      CHECK_EQ(roo_transceivers_Quantity_kTemperature, m.quantity());
    }
    return Reading{.value = roo_quantity::TemperatureDegCelcius(m.value()),
                   .time = m.time()};
  }

 private:
  roo_transceivers::BoundSensor bound_sensor_;
};

}  // namespace roo_control
