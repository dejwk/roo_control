#include "roo_control/selector/push_button.h"

namespace roo_control {

using roo_time::Interval;
using roo_time::Millis;
using roo_time::Uptime;

static constexpr Interval kDebounceInterval = Millis(20);
static constexpr Interval kLongPressInterval = Millis(500);
static constexpr Interval kDoubleClickDeadline = Millis(500);

PushButton::PushButton(BinarySelector& selector, BinaryLogicalState idle_state)
    : selector_(selector),
      idle_state_(idle_state),
      state_(selector.getState()),
      last_state_change_time_(Uptime::Now()),
      prev_state_change_time_(last_state_change_time_),
      last_click_time_(last_state_change_time_),
      is_pressed_(state_ != idle_state) {}

void PushButton::tick() {
  Uptime now = Uptime::Now();
  if (now - last_state_change_time_ < kDebounceInterval) return;
  BinaryLogicalState new_state = selector_.getState();
  if (new_state == state_) {
    // Maybe long-press?
    if (is_pressed_ && now - last_state_change_time_ > kLongPressInterval) {
      onLongPress();
      is_pressed_ = false;
    }
    return;
  }
  prev_state_change_time_ = last_state_change_time_;
  last_state_change_time_ = now;
  state_ = new_state;
  if (state_ != idle_state_) {
    is_pressed_ = true;
    onDown();
  } else {
    onUp();
    if (is_pressed_) {
      if (now - last_click_time_ < kDoubleClickDeadline) {
        onDoubleClick();
      } else {
        onClick();
      }
      last_click_time_ = now;
      is_pressed_ = false;
    }
  }
}

}  // namespace roo_control