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
using SensorId = uint32_t;
using ActuatorId = uint32_t;

class UniversalTransceiverDeviceId {
 public:
  UniversalTransceiverDeviceId() : family_(0), uid_(0) {}

  bool isDefined() const { return family_ != kNoDevice; }

  UniversalTransceiverDeviceId(uint32_t family, uint64_t uid)
      : family_(family), uid_(uid) {}

  TransceiverFamilyId family() const { return family_; }
  TransceiverDeviceId uid() const { return uid_; }

 private:
  TransceiverFamilyId family_;
  TransceiverDeviceId uid_;
};

inline bool operator==(const UniversalTransceiverDeviceId& a,
                       const UniversalTransceiverDeviceId& b) {
  return a.family() == b.family() && a.uid() == b.uid();
}

struct UniversalTransceiverDeviceIdHashFn {
  uint32_t operator()(const UniversalTransceiverDeviceId& val) const {
    return ((val.uid() >> 32) * 5 + val.uid() * 11 + val.family() * 17);
  }
};

class UniversalSensorId {
 public:
  UniversalSensorId() : device_id_(), sensor_id_(0) {}

  bool isDefined() const { return device_id_.isDefined(); }

  UniversalSensorId(uint32_t family, uint64_t uid, uint32_t sensor_id)
      : device_id_(family, uid), sensor_id_(sensor_id) {}

  TransceiverFamilyId family() const { return device_id_.family(); }
  TransceiverDeviceId device() const { return device_id_.uid(); }
  SensorId sensor() const { return sensor_id_; }

 private:
  UniversalTransceiverDeviceId device_id_;
  SensorId sensor_id_;
};

inline bool operator==(const UniversalSensorId& a, const UniversalSensorId& b) {
  return a.family() == b.family() && a.device() == b.device() &&
         a.sensor() == b.sensor();
}

struct UniversalSensorHashFn {
  uint32_t operator()(const UniversalSensorId& val) const {
    return (val.sensor() * 5 + val.device() * 61 + val.family() * 131);
  }
};

}  // namespace roo_control

namespace std {
template <>
struct hash<roo_control::UniversalTransceiverDeviceId> {
  size_t operator()(
      const roo_control::UniversalTransceiverDeviceId& val) const {
    return ((val.uid() >> 32) * 5 + val.uid() * 11 + val.family() * 17);
  }
};

}  // namespace std
