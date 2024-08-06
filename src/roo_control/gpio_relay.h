#include <Arduino.h>

#include "roo_control/switch.h"

namespace roo_control {

// Simple, raw, Arduino GPIO-pin-controlled relay switch.
class GpioRelay : public BinarySwitch {
 public:
  GpioRelay(uint8_t pin) : pin_(pin) { pinMode(pin_, OUTPUT); }

  bool setState(BinarySwitchState state) override {
    digitalWrite(pin_, state);
    return digitalRead(pin_) == state;
  }

  BinarySwitchState getState() override {
    return (BinarySwitchState)digitalRead(pin_);
  }

 private:
  uint8_t pin_;
};

}  // namespace roo_control