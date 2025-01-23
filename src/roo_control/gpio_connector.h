#include <Arduino.h>

#include "roo_control/selector.h"

namespace roo_control {

// Simple, raw, Arduino GPIO-pin-connected logical sensor.
class GpioConnector : public BinarySelector {
 public:
  GpioConnector(uint8_t pin, int pull_mode = 0) : pin_(pin) {
    pinMode(pin_, INPUT | pull_mode);
  }

  BinaryLogicalState getState() const override {
    return (BinaryLogicalState)digitalRead(pin_);
  }

 private:
  uint8_t pin_;
};

}  // namespace roo_control