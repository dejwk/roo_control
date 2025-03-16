#pragma once

#include "roo_control/selector/selector.h"
#include "roo_time.h"

namespace roo_control {

// Represents a human-pressable monentary button.
//
// This class implements debouncing and provides simple form of 'gesture'
// detection.
class PushButton : public BinarySelector {
 public:
  PushButton(BinarySelector& selector,
             BinaryLogicalState idle_state = BINARY_STATE_HIGH);

  BinaryLogicalState getState() const override { return state_; }

  // Needs to be called periodically, and frequently (every 5-10 ms).
  void tick();

 protected:
  // Overridable methods below are called from tick(). If you call tick() from a
  // time-sensitive context, you may want to hand off the events to a queue in
  // the implementation of these methods.

  virtual void onDown() {}
  virtual void onUp() {}
  virtual void onClick() {}
  virtual void onLongPress() {}
  virtual void onDoubleClick() {}

 private:
  BinarySelector& selector_;
  const BinaryLogicalState idle_state_;

  BinaryLogicalState state_;
  roo_time::Uptime last_state_change_time_;
  roo_time::Uptime prev_state_change_time_;
  roo_time::Uptime last_click_time_;
  bool is_pressed_;
};

}  // namespace roo_control