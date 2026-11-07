#pragma once

#include "roo_control/switch/switch.h"
#include "roo_scheduler.h"
#include "roo_time.h"

namespace roo_control {

/// Default backoff policy for InertSwitch retries.
roo_time::Duration DefaultBackoff(int retry_count);

/// Switch that adds inertia between state changes on top of a raw actuator.
///
/// Useful to counter-act bouncing that could damage physical relays.
///
/// This switch provides some degree of fault tolerance: as long as the actuator
/// reports false from setState(), the inert switch will keep retrying calling
/// it. (You can specify a retry policy for doing so; the default policy is a
/// randomized exponential backoff). However, when the actuator reports true
/// from setState(), the inert switch trusts it to enforce the setting.
///
/// If you need stronger fault tolerance, e.g. when the switch is remotely
/// controlled, consider using a FaultTolerantSwitch.
template <typename StateT>
class InertSwitch : public Switch<StateT> {
 public:
  using State = StateT;

  InertSwitch(roo_scheduler::Scheduler& scheduler, Switch<StateT>& actuator,
              roo_time::Duration inertia = roo_time::Millis(500))
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

  /// Returns the actual state the switch is currently at.
  bool getState(State& result) const override {
    return actuator_.getState(result);
  }

  /// Returns the state that the switch has been requested to take.
  ///
  /// The actual state may lag behind due to inertia. Returns false if the
  /// intended state has never been set yet.
  bool getIntendedState(State& state) const {
    if (!initialized_) return false;
    state = intended_state_;
    return true;
  }

  /// Sets the intended state of the switch.
  ///
  /// If the intended state of the switch was already set to the same value,
  /// returns immediately. Otherwise, if the last state change was more ago
  /// than the `inertia` interval, the state change is immediately attempted by
  /// calling setState() on the actuator. Otherwise, the state change attempt is
  /// scheduled to occur after the `inertia` interval since the last change.
  ///
  /// In case that setting the state fails, a retry is scheduled, according to
  /// the retry policy specified at creation time (by default, a randomized
  /// exponential backoff, truncated at 5s). The retries continue until
  /// `actuator.setState()` returns true.
  bool setState(State state) override {
    if (initialized_ && intended_state_ == state) {
      return true;
    }
    intended_state_ = state;
    initialized_ = true;
    // Note: if the requested state is different than the current
    // intended_state_, the actuator may be in the process of delivering the
    // previous state change, and the observed state might not yet reflect it.
    // Therefore, we never rely on the observed state and always call the
    // actuator.
    roo_time::Uptime now = roo_time::Uptime::Now();
    roo_time::Uptime deferred_set_time = when_switched_ + inertia_;
    if (deferred_set_time <= now) {
      // The minimums have been met; set immediately.
      set();
    } else {
      // Unless already scheduled to update, schedule to update with delay.
      if (!deferred_set_pending_) {
        deferred_set_pending_ = true;
        scheduler_.scheduleOn(deferred_set_time, deferred_setter_,
                              roo_scheduler::PRIORITY_ELEVATED);
      }
    }
    stateChanged();
    return true;
  }

  /// Returns true if a deferred update is pending.
  bool hasPendingChange() const { return deferred_set_pending_; }

  /// Returns the time of last actual state change.
  roo_time::Uptime whenSwitched() const { return when_switched_; }

  /// Returns the inertia interval.
  roo_time::Duration intertia() const { return inertia_; }

 protected:
  /// Can be overridden to receive state change notifications.
  ///
  /// Triggers when either the intended state changes, or setState() on the
  /// actuator succeeds (confirming update request of the actual state), or
  /// both.
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
    roo_time::Duration delay = backoff_policy_(failure_count_);
    scheduler_.scheduleAfter(delay, deferred_setter_,
                             roo_scheduler::PRIORITY_ELEVATED);
    return false;
  }

  // Called by the scheduler to execute the deferred state change.
  void deferredSet() {
    deferred_set_pending_ = false;
    if (set()) {
      stateChanged();
    }
  }

  /// Used to schedule deferred state updates.
  roo_scheduler::Scheduler& scheduler_;

  /// The underlying switch that we're driving.
  Switch<State>& actuator_;

  /// The minimum interval between state changes to impose on the actuator.
  roo_time::Duration inertia_;

  /// The retry policy to handle set failures on the actuator.
  std::function<roo_time::Duration(int retry_count)> backoff_policy_;

  /// Helper to execute the scheduled deferred set.
  roo_scheduler::Task deferred_setter_;

  /// Whether a deferred set is scheduled.
  bool deferred_set_pending_;

  /// True when the intended state has been set at least once.
  bool initialized_;

  /// The intended state of the switch.
  State intended_state_;

  /// The time when the actual state has most recently changed.
  roo_time::Uptime when_switched_;

  /// Failures since the last successful state reconciliation.
  int failure_count_;
};

/// Convenience alias for a binary inert switch.
using InertBinarySwitch = InertSwitch<BinaryLogicalState>;

}  // namespace roo_control