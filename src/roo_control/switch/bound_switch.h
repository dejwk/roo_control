#pragma once

#include "roo_control/switch/switch.h"
#include "roo_transceivers/binding/binding.h"

namespace roo_control {

template <typename State>
class BoundSwitch : public Switch<State> {
 public:
  BoundSwitch(roo_transceivers::Universe& universe,
              const roo_transceivers::ActuatorBinding* binding)
      : bound_sensing_actuator_(universe, binding) {}

  bool getState(State& result) const override {
    roo_transceivers::Measurement m = bound_sensing_actuator_.read();
    if (m.isDefined()) {
      CHECK(m.quantity() == roo_transceivers_Quantity_kBinaryState ||
            m.quantity() == roo_transceivers_Quantity_kMultiState);
      if ((int)m.value() != m.value()) {
        LOG_EVERY_T(ERROR, 1)
            << "Selector value is not an integer: " << m.value();
        return false;
      }
      result = (State)m.value();
      return true;
    }
    // LOG(ERROR) << "Unknown selector state " << bound_sensing_actuator_;
    return false;
  }

  bool setState(State state) override {
    return bound_sensing_actuator_.write((float)state);
  }

 private:
  roo_transceivers::BoundSensingActuator bound_sensing_actuator_;
};

template <>
class BoundSwitch<BinaryLogicalState> : public Switch<BinaryLogicalState> {
 public:
  BoundSwitch(roo_transceivers::Universe& universe,
              const roo_transceivers::ActuatorBinding* binding)
      : bound_sensing_actuator_(universe, binding) {}

  bool getState(BinaryLogicalState& result) const override {
    roo_transceivers::Measurement m = bound_sensing_actuator_.read();
    if (m.isDefined()) {
      if (m.quantity() != roo_transceivers_Quantity_kBinaryState &&
          m.quantity() != roo_transceivers_Quantity_kMultiState) {
        LOG_EVERY_T(ERROR, 1)
            << "Unexpected quantity when reading bound actuator: "
            << m.quantity() << " " << m.value();
        return false;
      }
      if (m.value() != 0.0f && m.value() != 1.0f) {
        LOG_EVERY_T(ERROR, 1) << "Selector value is invalid: " << m.value();
        return false;
      }
      result = (BinaryLogicalState)((int)m.value());
      return true;
    }
    // LOG(ERROR) << "Unknown selector state " << bound_sensing_actuator_;
    return false;
  }

  bool setState(BinaryLogicalState state) override {
    return bound_sensing_actuator_.write((float)state);
  }

 private:
  roo_transceivers::BoundSensingActuator bound_sensing_actuator_;
};

using BoundBinarySwitch = BoundSwitch<BinaryLogicalState>;

}  // namespace roo_control
