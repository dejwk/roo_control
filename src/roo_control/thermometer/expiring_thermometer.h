#pragma once

#include "roo_control/thermometer/thermometer.h"

namespace roo_control {

/// Reports readings of another thermometer if fresher than an expiration.
///
/// Otherwise, reports Unknown.
class ExpiringThermometer : public Thermometer {
 public:
  ExpiringThermometer(const Thermometer *thermometer)
      : ExpiringThermometer(thermometer, roo_time::Hours(10000000)) {}

  ExpiringThermometer(const Thermometer *thermometer,
                      roo_time::Duration expiration)
      : thermometer_(thermometer), expiration_(expiration), cached_() {}

  void setExpiration(roo_time::Duration expiration) {
    expiration_ = expiration;
  }

  roo_time::Duration expiration() const { return expiration_; }

  Reading readTemperature() const override {
    Reading reading = thermometer_->readTemperature();
    if (reading.time > cached_.time && cached_.value.isUnknown()) {
      cached_ = reading;
    }
    if (reading.value.isUnknown() && !cached_.value.isUnknown()) {
      reading = cached_;
    }
    if (reading.time + expiration_ < roo_time::Uptime::Now()) {
      reading.value = roo_quantity::UnknownTemperature();
    }
    return reading;
  }

 private:
  const Thermometer *thermometer_;
  roo_time::Duration expiration_;
  mutable Reading cached_;
};

/// Convenience function for expiring thermometer readings.
///
/// Returns the temperature if it is fresher than the expiration threshold, and
/// Unknown otherwise. Useful when stale thermometer readings (e.g. due to
/// disconnected devices) should not be used.
inline Thermometer::Reading ReadExpiringTemperature(
    const Thermometer &t, roo_time::Duration expiration) {
  return ExpiringThermometer(&t, expiration).readTemperature();
}

}  // namespace roo_control