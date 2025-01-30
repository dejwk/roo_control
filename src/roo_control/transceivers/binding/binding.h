#pragma once

#include <cstdint>

#include "roo_control/transceivers/binding/hal/store.h"
#include "roo_control/transceivers/measurement.h"
#include "roo_control/transceivers/universe.h"

namespace roo_control {

class SensorBinding {
 public:
  SensorBinding(SensorBindingStore& store, SensorBindingStore::Key key)
      : id_(), store_(store), key_(key), synced_(false) {}

  UniversalTransceiverDeviceId get() const {
    sync();
    return id_;
  }

  bool isBound() const {
    sync();
    return id_.isDefined();
  }

  void bind(UniversalTransceiverDeviceId id) {
    if (id_ == id) return;
    id_ = id;
    if (id_ == UniversalTransceiverDeviceId()) {
      store_.clearBinding(key_);
    } else {
      store_.setBinding(key_, id_);
    }
    synced_ = true;
  }

  void unbind() {
    id_ = UniversalTransceiverDeviceId();
    store_.clearBinding(key_);
    synced_ = true;
  }

 private:
  void sync() const {
    if (!synced_) {
      id_ = store_.getBinding(key_);
      synced_ = true;
    }
  }
  mutable UniversalTransceiverDeviceId id_;

  SensorBindingStore& store_;
  SensorBindingStore::Key key_;

  mutable bool synced_;
};

class BoundThermometer : public roo_temperature::Thermometer {
 public:
  BoundThermometer(SensorUniverse& universe, const SensorBinding& binding)
      : universe_(universe), binding_(&binding) {}

  Reading readTemperature() const override {
    Measurement m = universe_.read(binding_->get());
    if (!m.isDefined()) {
      return Reading{.value = roo_temperature::Unknown(),
                     .time = roo_time::Uptime::Start()};
    }
    CHECK_EQ(roo_control_Quantity_kTemperature, m.quantity());
    return Reading{.value = roo_temperature::DegCelcius(m.value()),
                   .time = m.time()};
  }

 private:
  SensorUniverse& universe_;
  const SensorBinding* binding_;
};

}  // namespace roo_control