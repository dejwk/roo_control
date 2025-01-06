#pragma once

#include <cstdint>

#include "roo_control/sensors/binding/hal/store.h"
#include "roo_control/sensors/universe.h"

namespace roo_control {

class SensorBinding {
 public:
  SensorBinding(SensorBindingStore& store, SensorBindingStore::Key key)
      : id_(), store_(store), key_(key), synced_(false) {}

  UniversalDeviceId get() const {
    sync();
    return id_;
  }

  bool isBound() const {
    sync();
    return id_.isDefined();
  }

  void bind(UniversalDeviceId id) {
    if (id_ == id) return;
    id_ = id;
    if (id_ == UniversalDeviceId()) {
      store_.clearBinding(key_);
    } else {
      store_.setBinding(key_, id_);
    }
    synced_ = true;
  }

  void unbind() {
    if (id_ == UniversalDeviceId()) return;
    id_ = UniversalDeviceId();
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
  mutable UniversalDeviceId id_;

  SensorBindingStore& store_;
  SensorBindingStore::Key key_;

  mutable bool synced_;
};

class BoundThermometer : public roo_temperature::Thermometer {
 public:
  BoundThermometer(SensorUniverse& universe, SensorBinding& binding)
      : universe_(universe), binding_(binding) {}

  Reading readTemperature() const override {
    return universe_.readTemperature(binding_.get());
  }

 private:
  SensorUniverse& universe_;
  SensorBinding& binding_;
};

}  // namespace roo_control