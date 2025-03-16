#pragma once

#include <string>

#include "roo_collections/hash.h"
#include "roo_collections/small_string.h"
#include "roo_logging.h"

namespace roo_control {

using TransceiverDeviceSchema = roo_collections::SmallString<16>;
using TransceiverDeviceId = roo_collections::SmallString<24>;
using TransceiverSensorId = roo_collections::SmallString<24>;
using TransceiverActuatorId = roo_collections::SmallString<24>;

class TransceiverDeviceLocator {
 public:
  TransceiverDeviceLocator();

  TransceiverDeviceLocator(roo::string_view schema, roo::string_view device_id);

  const TransceiverDeviceSchema& schema() const { return schema_; }
  const TransceiverDeviceId& device_id() const { return device_id_; }

  bool isDefined() const { return !schema_.empty(); }

  void write_cstr(char* buf) const;

 private:
  TransceiverDeviceSchema schema_;
  TransceiverDeviceId device_id_;
};

inline bool operator==(const TransceiverDeviceLocator& a,
                       const TransceiverDeviceLocator& b) {
  return a.schema() == b.schema() && a.device_id() == b.device_id();
}

inline bool operator!=(const TransceiverDeviceLocator& a,
                       const TransceiverDeviceLocator& b) {
  return !(a == b);
}

class TransceiverSensorLocator {
 public:
  TransceiverSensorLocator();

  TransceiverSensorLocator(roo::string_view schema, roo::string_view device_id,
                           roo::string_view sensor_id);

  TransceiverSensorLocator(const TransceiverDeviceLocator& device_loc,
                           roo::string_view sensor_id);

  const TransceiverDeviceLocator& device_locator() const {
    return device_locator_;
  }

  const TransceiverDeviceSchema& schema() const {
    return device_locator_.schema();
  }

  const TransceiverDeviceId& device_id() const {
    return device_locator_.device_id();
  }

  const TransceiverSensorId& sensor_id() const { return sensor_id_; }

  bool isDefined() const { return device_locator().isDefined(); }

  void write_cstr(char* buf) const;

 private:
  TransceiverDeviceLocator device_locator_;
  TransceiverSensorId sensor_id_;
};

inline bool operator==(const TransceiverSensorLocator& a,
                       const TransceiverSensorLocator& b) {
  return a.device_locator() == b.device_locator() &&
         a.sensor_id() == b.sensor_id();
}

class TransceiverActuatorLocator {
 public:
  TransceiverActuatorLocator();

  TransceiverActuatorLocator(const TransceiverDeviceLocator& device_loc,
                             roo::string_view actuator_id);

  TransceiverActuatorLocator(roo::string_view schema,
                             roo::string_view device_id,
                             roo::string_view actuator_id);

  const TransceiverDeviceLocator& device_locator() const {
    return device_locator_;
  }

  const TransceiverDeviceSchema& schema() const {
    return device_locator_.schema();
  }

  const TransceiverDeviceId& device_id() const {
    return device_locator_.device_id();
  }

  const TransceiverActuatorId& actuator_id() const { return actuator_id_; }

  bool isDefined() const { return device_locator().isDefined(); }

  void write_cstr(char* buf) const;

 private:
  TransceiverDeviceLocator device_locator_;
  TransceiverActuatorId actuator_id_;
};

inline bool operator==(const TransceiverActuatorLocator& a,
                       const TransceiverActuatorLocator& b) {
  return a.device_locator() == b.device_locator() &&
         a.actuator_id() == b.actuator_id();
}

}  // namespace roo_control

namespace std {

template <>
struct hash<roo_control::TransceiverDeviceSchema> {
  size_t operator()(const roo_control::TransceiverDeviceSchema& schema) const {
    return roo_collections::murmur3_32(schema.c_str(), strlen(schema.c_str()),
                                       0);
  }
};

template <>
struct hash<roo_control::TransceiverDeviceLocator> {
  size_t operator()(const roo_control::TransceiverDeviceLocator& loc) const {
    return roo_collections::murmur3_32(
        loc.device_id().c_str(), strlen(loc.device_id().c_str()),
        std::hash<roo_control::TransceiverDeviceSchema>()(loc.schema()));
  }
};

template <>
struct hash<roo_control::TransceiverSensorLocator> {
  size_t operator()(const roo_control::TransceiverSensorLocator& loc) const {
    return roo_collections::murmur3_32(
        loc.sensor_id().c_str(), strlen(loc.sensor_id().c_str()),
        std::hash<roo_control::TransceiverDeviceLocator>()(
            loc.device_locator()));
  }
};

template <>
struct hash<roo_control::TransceiverActuatorLocator> {
  size_t operator()(const roo_control::TransceiverActuatorLocator& loc) const {
    return roo_collections::murmur3_32(
        loc.actuator_id().c_str(), strlen(loc.actuator_id().c_str()),
        std::hash<roo_control::TransceiverDeviceLocator>()(
            loc.device_locator()));
  }
};

}  // namespace std

roo_logging::Stream& operator<<(
    roo_logging::Stream& s, const roo_control::TransceiverDeviceSchema& schema);

roo_logging::Stream& operator<<(
    roo_logging::Stream& s, const roo_control::TransceiverDeviceLocator& loc);

roo_logging::Stream& operator<<(
    roo_logging::Stream& s, const roo_control::TransceiverSensorLocator& loc);

roo_logging::Stream& operator<<(
    roo_logging::Stream& s, const roo_control::TransceiverActuatorLocator& loc);
