#pragma once

#include <cstdint>
#include <vector>  // for std::hash.

namespace roo_control {

enum PredefinedFamily {
  kNoDevice = 0,
  kOneWire = 1,
};

using SensorFamilyId = uint32_t;
using SensorDeviceId = uint64_t;

class UniversalDeviceId {
 public:
  UniversalDeviceId() : family_(0), uid_(0) {}

  bool isDefined() const { return family_ != kNoDevice; }

  UniversalDeviceId(uint32_t family, uint64_t uid)
      : family_(family), uid_(uid) {}

  SensorFamilyId family() const { return family_; }
  SensorDeviceId uid() const { return uid_; }

 private:
  SensorFamilyId family_;
  SensorDeviceId uid_;
};

inline bool operator==(const UniversalDeviceId& a, const UniversalDeviceId& b) {
  return a.family() == b.family() && a.uid() == b.uid();
}

struct UniversalDeviceIdHashFn {
  uint32_t operator()(const UniversalDeviceId& val) const {
    return ((val.uid() >> 32) * 5 + val.uid() * 11 + val.family() * 17);
  }
};

}  // namespace roo_control

namespace std {
template <>
struct hash<roo_control::UniversalDeviceId> {
  size_t operator()(const roo_control::UniversalDeviceId& val) const {
    return ((val.uid() >> 32) * 5 + val.uid() * 11 + val.family() * 17);
  }
};
}  // namespace std
