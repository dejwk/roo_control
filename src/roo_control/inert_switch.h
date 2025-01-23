#pragma once

#include "roo_control/switch.h"
#include "roo_scheduler.h"
#include "roo_time.h"

namespace roo_control {

roo_time::Interval DefaultBackoff(int retry_count);

// Switch that adds intertia between state changes, on top of a 'raw' underlying
// switch, called an 'actuator'. Useful to counter-act bouncing that could
// damage physical relays.
template <typename StateT>
class InertSwitch : public Switch<StateT> {
 public:
  using State = StateT;

  InertSwitch(roo_scheduler::Scheduler& scheduler, Switch<StateT>& actuator,
              roo_time::Interval inertia = roo_time::Millis(500))
      : scheduler_(scheduler),
        actuator_(actuator),
        inertia_(inertia),
        backoff_policy_(DefaultBackoff),
        deferred_setter_([this]() { deferredSet(); }),
        deferred_set_pending_(false),
        initialized_(false),
        when_switched_(roo_time::Uptime::Now()),
        failure_count_(0) {}

  virtual ~InertSwitch() = default;

  // Returns the actual state the switch is currently at.
  State getState() const override { return actuator_.getState(); }

  // Returns the state that the switch has been requested to take. The actual
  // state may lag behind due to inertia.
  State getIntendedState() const {
    return initialized_ ? intended_state_ : getState();
  }

  // Sets the intended state of the switch. If the switch already is in that
  // state, returns immediately. Otherwise, if the last state change was more
  // ago than the `inertia` interval, the state is immediately changed.
  // Otherwise, schedules the state change to occur after the `inertia` interval
  // since the last change.
  //
  // In case that setting the state fails, a retry is scheduled (with
  // exponential backoff). The implementation will keep trying to set the actual
  // state to the intended state until successful.
  //
  // This method always returns true.
  bool setState(State state) override {
    if (initialized_ && intended_state_ == state) return true;
    intended_state_ = state;
    initialized_ = true;
    if (getState() == intended_state_) {
      stateChanged();
      return true;
    }
    roo_time::Uptime now = roo_time::Uptime::Now();
    roo_time::Uptime deferred_set_time = when_switched_ + inertia_;
    if (deferred_set_time <= now) {
      // The minimums have been met; set immediately.
      set();
    } else {
      // Unless already scheduled to update, schedule to update with delay.
      if (!deferred_set_pending_) {
        deferred_set_pending_ = true;
        scheduler_.scheduleOn(&deferred_setter_, deferred_set_time);
      }
    }
    stateChanged();
    return true;
  }

  // Returns true if the intended and the actual state of the switch is
  // currently inconsistent, with deferred update pending.
  bool hasPendingChange() const { return deferred_set_pending_; }

  // Returns the time of last (actual) state change.
  roo_time::Uptime whenSwitched() const { return when_switched_; }

  // Returns the intertia interval.
  roo_time::Interval intertia() const { return inertia_; }

 protected:
  // Can be overridden to receive state change notifications. Triggers when
  // either the intended or the actual state, or both, change.
  virtual void stateChanged() const {}

 private:
  bool set() {
    if (actuator_.setState(intended_state_)) {
      when_switched_ = roo_time::Uptime::Now();
      failure_count_ = 0;
      return true;
    }
    ++failure_count_;
    deferred_set_pending_ = true;
    roo_time::Interval delay = backoff_policy_(failure_count_);
    scheduler_.scheduleAfter(&deferred_setter_, delay);
    return false;
  }

  // Called by the scheduler to execute the deferred state change.
  void deferredSet() {
    deferred_set_pending_ = false;
    if (actuator_.getState() == intended_state_) return;
    if (set()) {
      stateChanged();
    }
  }

  // Used to schedule deferred state updates.
  roo_scheduler::Scheduler& scheduler_;

  // The underlying switch that we're driving.
  Switch<State>& actuator_;

  // The minimum interval between state changes wa want to impose on the
  // actuator.
  roo_time::Interval inertia_;

  // The retry policy to handle set failures on the actuator.
  std::function<roo_time::Interval(int retry_count)> backoff_policy_;

  // Helper to execute the scheduled deferred set.
  roo_scheduler::Task deferred_setter_;

  // Whether there is a deferred set scheduled (or, equivalently, when the
  // intended state is different than the actual state).
  bool deferred_set_pending_;

  // True when the intended state has been set at least once.
  bool initialized_;

  // The intended state of the switch.
  State intended_state_;

  // The time when the actual state has most recently changed.
  roo_time::Uptime when_switched_;

  // How many failures actuator_.setState() has experienced since the last
  // succesful state reconcillation.
  int failure_count_;
};

using InertBinarySwitch = InertSwitch<BinaryLogicalState>;

}  // namespace roo_control