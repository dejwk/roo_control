#pragma once

#include <Arduino.h>

#include "roo_control/switch/switch.h"

namespace roo_control {

// Simple, raw, Arduino GPIO-pin-controlled relay switch.
class GpioRelay : public BinarySwitch {
 public:
  GpioRelay(uint8_t pin, BinaryLogicalState initial_state = BINARY_STATE_LOW)
      : pin_(pin) {
    pinMode(pin_, OUTPUT);
    digitalWrite(pin_, initial_state);
  }

  bool setState(BinaryLogicalState state) override {
    digitalWrite(pin_, state);
    return digitalRead(pin_) == state;
  }

  bool getState(BinaryLogicalState& result) const override {
    result = (BinaryLogicalState)digitalRead(pin_);
    return true;
  }

 private:
  uint8_t pin_;
};

}  // namespace roo_control