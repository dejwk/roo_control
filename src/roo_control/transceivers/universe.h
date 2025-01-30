#pragma once

#include <vector>

#include "roo_collections/flat_small_hash_map.h"
#include "roo_control/transceivers/family.h"
#include "roo_control/transceivers/id.h"
#include "roo_logging.h"

namespace roo_control {

// Represents a collection of dynamically managed sensors of the same functional
// type, but potentially different implementations, managed by separate
// 'families'. Example: a universe of temperature sensors, that can include the
// 'OneWire' and 'Roo-EspNow' families.
class SensorUniverse {
 public:
  struct FamilySpec {
    TransceiverFamilyId id;
    TransceiverFamily& family;
  };

  SensorUniverse() {}

  SensorUniverse(std::vector<FamilySpec> families) {
    for (const auto& family : families) {
      addFamily(family.id, family.family);
    }
  }

  void addFamily(TransceiverFamilyId id, TransceiverFamily& family) {
    CHECK(!family_index_.contains(id))
        << "Family " << id << " already registered.";
    family_index_[id] = families_.size();
    families_.emplace_back(id, &family);
  }

  // Returns the number of registered families.
  size_t familyCount() const { return families_.size(); }

  // Returns the i-th family.
  const TransceiverFamily& family(size_t idx) const {
    return *families_[idx].family;
  }

  // Returns the i-th family ID.
  TransceiverFamilyId family_id(size_t idx) const { return families_[idx].id; }

  Measurement read(UniversalSensorId id) const {
    auto it = family_index_.find(id.family());
    if (it == family_index_.end()) {
      return Measurement();
    }
    int pos = it->second;
    return families_[it->second].family->read(id.device(), id.sensor());
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
  void addEventListener(TransceiverEventListener* listener) {
    for (auto& family : families_) {
      family.family->addEventListener(listener);
    }
  }

  // Removes a previously registered listener.
  void removeEventListener(TransceiverEventListener* listener) {
    for (auto& family : families_) {
      family.family->removeEventListener(listener);
    }
  }

  // Generates a human-friendly, GUI-suitable name corresponding to the
  // specified device ID.
  std::string sensorUserFriendlyName(UniversalSensorId id) const {
    auto it = family_index_.find(id.family());
    if (it == family_index_.end()) {
      return "<invalid>";
    }
    int pos = it->second;
    return families_[it->second].family->sensorUserFriendlyName(id.device(),
                                                                id.sensor());
  }

 private:
  struct FamilyInfo {
    FamilyInfo(TransceiverFamilyId id, TransceiverFamily* family)
        : id(id), family(family) {}

    TransceiverFamilyId id;
    TransceiverFamily* family;
  };

  std::vector<FamilyInfo> families_;
  roo_collections::FlatSmallHashMap<TransceiverFamilyId, int> family_index_;
};

}  // namespace roo_control