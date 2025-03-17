#pragma once

#include <cstdint>

#include "roo_control/transceivers/binding/hal/store.h"
#include "roo_control/transceivers/measurement.h"
#include "roo_control/transceivers/universe.h"
#include "roo_logging.h"

namespace roo_control {

class SensorBinding {
 public:
  SensorBinding(TransceiverBindingStore& store,
                TransceiverBindingStore::SensorKey key)
      : loc_(), store_(store), key_(key), synced_(false) {}

  TransceiverSensorLocator get() const {
    sync();
    return loc_;
  }

  bool isBound() const {
    sync();
    return loc_.isDefined();
  }

  void bind(const TransceiverSensorLocator& loc) {
    if (loc_ == loc) return;
    loc_ = loc;
    if (!loc_.isDefined()) {
      store_.clearSensorBinding(key_);
    } else {
      store_.setSensorBinding(key_, loc_);
    }
    synced_ = true;
  }

  void unbind() {
    loc_ = TransceiverSensorLocator();
    store_.clearSensorBinding(key_);
    synced_ = true;
  }

 private:
  friend roo_logging::Stream& operator<<(roo_logging::Stream& os, const SensorBinding& binding);

  void sync() const {
    if (!synced_) {
      loc_ = store_.getSensorBinding(key_);
      synced_ = true;
    }
  }
  mutable TransceiverSensorLocator loc_;

  TransceiverBindingStore& store_;
  TransceiverBindingStore::SensorKey key_;

  mutable bool synced_;
};

class BoundSensor {
 public:
  BoundSensor(TransceiverUniverse& universe, const SensorBinding& binding)
      : universe_(universe), binding_(&binding) {}

  Measurement read() const {
    TransceiverSensorLocator loc = binding_->get();
    if (loc.isDefined()) {
      return universe_.read(binding_->get());
    }
    return Measurement();
  }

 private:
  friend roo_logging::Stream& operator<<(roo_logging::Stream& os, const BoundSensor& sensor);

  TransceiverUniverse& universe_;
  const SensorBinding* binding_;
};

}  // namespace roo_control