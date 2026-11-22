#pragma once

#include <climits>

#include "roo_control/selector/selector.h"
#include "roo_transceivers.h"
#include "roo_transceivers/binding/binding.h"

namespace roo_control {

/// Selector backed by a bound sensor from roo_transceivers.
template <typename State>
class BoundSelector : public Selector<State> {
 public:
  /// Creates a selector that reads @p binding through @p universe.
  /// Both must outlive this selector.
  BoundSelector(roo_transceivers::Universe& universe,
                const roo_transceivers::SensorBinding& binding)
      : bound_sensor_(universe, &binding) {}

  /// Reads an integer state, returning false for missing or invalid values.
  bool getState(State& result) const override {
    const roo_transceivers::Measurement reading = bound_sensor_.read();
    if (!reading.isDefined()) {
      LOG(ERROR) << "Unknown selector state " << bound_sensor_;
      return false;
    }
    CHECK(reading.quantity() == roo_transceivers::Quantity::kBinaryState ||
          reading.quantity() == roo_transceivers::Quantity::kMultiState);
    const double value = reading.value();
    if (value < INT_MIN || value > INT_MAX ||
        static_cast<int>(value) != value) {
      LOG(ERROR) << "Selector value is not a representable integer: " << value;
      return false;
    }
    result = static_cast<State>(static_cast<int>(value));
    return true;
  }

 private:
  roo_transceivers::BoundSensor bound_sensor_;
};

/// BinaryLogicalState specialization with stricter value checks.
template <>
class BoundSelector<BinaryLogicalState> : public Selector<BinaryLogicalState> {
 public:
  /// Creates a binary selector that reads @p binding through @p universe.
  /// Both must outlive this selector.
  BoundSelector(roo_transceivers::Universe& universe,
                const roo_transceivers::SensorBinding& binding)
      : bound_sensor_(universe, &binding) {}

  /// Reads a binary state, returning false for missing or invalid values.
  bool getState(BinaryLogicalState& result) const override {
    const roo_transceivers::Measurement reading = bound_sensor_.read();
    if (!reading.isDefined()) {
      LOG(ERROR) << "Unknown selector state " << bound_sensor_;
      return false;
    }
    CHECK(reading.quantity() == roo_transceivers::Quantity::kBinaryState ||
          reading.quantity() == roo_transceivers::Quantity::kMultiState);
    const float value = reading.value();
    if (value != 0.0f && value != 1.0f) {
      LOG(ERROR) << "Selector value is invalid: " << value;
      return false;
    }
    result = value == 0.0f ? BINARY_STATE_LOW : BINARY_STATE_HIGH;
    return true;
  }

 private:
  roo_transceivers::BoundSensor bound_sensor_;
};

/// Convenience alias for a bound binary selector.
using BoundBinarySelector = BoundSelector<BinaryLogicalState>;

}  // namespace roo_control
