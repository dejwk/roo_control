#pragma once

#include <cstdint>
#include <vector>  // for std::hash.

namespace roo_control {

enum PredefinedTransceiverFamily {
  kNoDevice = 0,
  kOneWire = 1,
};

using TransceiverFamilyId = uint32_t;
using TransceiverDeviceId = uint64_t;
using SensorIdx = uint32_t;
using ActuatorIdx = uint32_t;

class UniversalTransceiverDeviceId {
 public:
  UniversalTransceiverDeviceId() : family_id_(0), device_id_(0) {}

  bool isDefined() const { return family_id_ != kNoDevice; }

  UniversalTransceiverDeviceId(uint32_t family_id, uint64_t device_id)
      : family_id_(family_id), device_id_(device_id) {}

  TransceiverFamilyId family_id() const { return family_id_; }
  TransceiverDeviceId device_id() const { return device_id_; }

 private:
  TransceiverFamilyId family_id_;
  TransceiverDeviceId device_id_;
};

inline bool operator==(const UniversalTransceiverDeviceId& a,
                       const UniversalTransceiverDeviceId& b) {
  return a.family_id() == b.family_id() && a.device_id() == b.device_id();
}

struct UniversalTransceiverDeviceIdHashFn {
  uint32_t operator()(const UniversalTransceiverDeviceId& val) const {
    return ((val.device_id() >> 32) * 5 + val.device_id() * 11 +
            val.family_id() * 17);
  }
};

class UniversalSensorId {
 public:
  UniversalSensorId() : device_id_(), sensor_idx_(0) {}

  bool isDefined() const { return device_id_.isDefined(); }

  UniversalSensorId(uint32_t family_id, uint64_t device_id, uint32_t sensor_id)
      : device_id_(family_id, device_id), sensor_idx_(sensor_id) {}

  TransceiverFamilyId family_id() const { return device_id_.family_id(); }
  TransceiverDeviceId device_id() const { return device_id_.device_id(); }
  SensorIdx sensor_idx() const { return sensor_idx_; }

 private:
  UniversalTransceiverDeviceId device_id_;
  SensorIdx sensor_idx_;
};

inline bool operator==(const UniversalSensorId& a, const UniversalSensorId& b) {
  return a.family_id() == b.family_id() && a.device_id() == b.device_id() &&
         a.sensor_idx() == b.sensor_idx();
}

struct UniversalSensorHashFn {
  uint32_t operator()(const UniversalSensorId& val) const {
    return (val.sensor_idx() * 5 + val.device_id() * 61 +
            val.family_id() * 131);
  }
};

}  // namespace roo_control

namespace std {
template <>
struct hash<roo_control::UniversalTransceiverDeviceId> {
  size_t operator()(
      const roo_control::UniversalTransceiverDeviceId& val) const {
    return ((val.device_id() >> 32) * 5 + val.device_id() * 11 +
            val.family_id() * 17);
  }
};

}  // namespace std
