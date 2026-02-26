#pragma once

#include "roo_control/selector/selector.h"
#include "roo_transceivers/binding/binding.h"

namespace roo_control {

/// Selector backed by a bound sensor from roo_transceivers.
template <typename State>
class BoundSelector : public Selector<State> {
 public:
  BoundSelector(roo_transceivers::Universe& universe,
                const roo_transceivers::SensorBinding& binding)
      : bound_sensor_(universe, binding), state_() {}

  bool getState(State& result) const override {
    Measurement m = bound_sensor_.read();
    if (m.isDefined()) {
      CHECK(m.quantity() == roo_control_Quantity_kBinaryState ||
            m.quantity() == roo_control_Quantity_kMultiState);
      if ((int)m.value()) != m.value() {
          LOG(ERROR) << "Selector value is not an integer: " << m.value();
          return false;
        }
      result = (State)m.value();
      return true;
    }
    LOG(ERROR) << "Unknown selector state " << bound_sensor_;
    return false;
  }

 private:
  roo_transceivers::BoundSensor bound_sensor_;
};

/// BinaryLogicalState specialization with stricter value checks.
template <>
class BoundSelector<BinaryLogicalState> : public Selector<BinaryLogicalState> {
 public:
  BoundSelector<BinaryLogicalState>(
      roo_transceivers::Universe& universe,
      const roo_transceivers::SensorBinding& binding)
      : bound_sensor_(universe, binding) {}

  bool getState(BinaryLogicalState& result) const override {
    roo_transceivers::Measurement m = bound_sensor_.read();
    if (m.isDefined()) {
      CHECK(m.quantity() == roo_control_Quantity_kBinaryState ||
            m.quantity() == roo_control_Quantity_kMultiState);
      if (m.value() != 0.0f && m.value() != 1.0f) {
        LOG(ERROR) << "Selector value is invalid: " << m.value();
        return false;
      }
      result = (BinaryLogicalState)((int)m.value());
      return true;
    }
    LOG(ERROR) << "Unknown selector state " << bound_sensor_;
    return false;
  }

 private:
  roo_transceivers::BoundSensor bound_sensor_;
};

/// Convenience alias for a bound binary selector.
using BoundBinarySelector = BoundSelector<BinaryLogicalState>;

}  // namespace roo_control
