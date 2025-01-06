#pragma once

#include <vector>

#include "roo_collections/flat_small_hash_map.h"
#include "roo_control/sensors/device_id.h"
#include "roo_control/sensors/family.h"
#include "roo_logging.h"

namespace roo_control {

// Represents a collection of dynamically managed sensors of the same functional
// type, but potentially different implementations, managed by separate
// 'families'. Example: a universe of temperature sensors, that can include the
// 'OneWire' and 'Roo-EspNow' families.
class SensorUniverse {
 public:
  struct FamilySpec {
    SensorFamilyId id;
    SensorFamily& family;
  };

  SensorUniverse() {}

  SensorUniverse(std::vector<FamilySpec> families) {
    for (const auto& family : families) {
      addFamily(family.id, family.family);
    }
  }

  void addFamily(SensorFamilyId id, SensorFamily& family) {
    CHECK(!family_index_.contains(id))
        << "Family " << id << " already registered.";
    family_index_[id] = families_.size();
    families_.emplace_back(id, &family);
  }

  // Returns the number of registered families.
  size_t familyCount() const { return families_.size(); }

  // Returns the i-th family.
  const SensorFamily& family(size_t idx) const {
    return *families_[idx].family;
  }

  // Returns the i-th family ID.
  SensorFamilyId family_id(size_t idx) const { return families_[idx].id; }

  roo_temperature::Thermometer::Reading readTemperature(
      UniversalDeviceId id) const {
    auto it = family_index_.find(id.family());
    if (it == family_index_.end()) {
      return roo_temperature::Thermometer::Reading{
          .value = roo_temperature::Temperature(),
          .time = roo_time::Uptime::Now(),
      };
    }
    int pos = it->second;
    return families_[it->second].family->readTemperature(id.uid());
  }

  // Requests sensor families that are able to do so, to update their state
  // (i.e. available sensors and their readings).
  void requestUpdate() {
    for (auto& family : families_) {
      family.family->requestUpdate();
    }
  }

  // Registers a listener to be notified when sensors changed or new readings
  // are available.
  void addEventListener(SensorEventListener* listener) {
    for (auto& family : families_) {
      family.family->addEventListener(listener);
    }
  }

  // Removes a previously registered listener.
  void removeEventListener(SensorEventListener* listener) {
    for (auto& family : families_) {
      family.family->removeEventListener(listener);
    }
  }

  // Generates a human-friendly, GUI-suitable name corresponding to the
  // specified device ID.
  std::string sensorUserFriendlyName(UniversalDeviceId id) const {
    auto it = family_index_.find(id.family());
    if (it == family_index_.end()) {
      return "<invalid>";
    }
    int pos = it->second;
    return families_[it->second].family->sensorUserFriendlyName(id.uid());
  }

 private:
  struct FamilyInfo {
    FamilyInfo(SensorFamilyId id, SensorFamily* family)
        : id(id), family(family) {}

    SensorFamilyId id;
    SensorFamily* family;
  };

  std::vector<FamilyInfo> families_;
  roo_collections::FlatSmallHashMap<SensorFamilyId, int> family_index_;
};

}  // namespace roo_control