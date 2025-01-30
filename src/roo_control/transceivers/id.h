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

inline bool operator==(const UniversalTransceiverDeviceId& a, const UniversalTransceiverDeviceId& b) {
  return a.family() == b.family() && a.uid() == b.uid();
}

struct UniversalTransceiverDeviceIdHashFn {
  uint32_t operator()(const UniversalTransceiverDeviceId& val) const {
    return ((val.uid() >> 32) * 5 + val.uid() * 11 + val.family() * 17);
  }
};

}  // namespace roo_control

namespace std {
template <>
struct hash<roo_control::UniversalTransceiverDeviceId> {
  size_t operator()(const roo_control::UniversalTransceiverDeviceId& val) const {
    return ((val.uid() >> 32) * 5 + val.uid() * 11 + val.family() * 17);
  }
};
}  // namespace std
