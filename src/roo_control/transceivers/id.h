#pragma once

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>  // for std::hash.

namespace roo_control {

class TransceiverDeviceSchema {
 public:
  TransceiverDeviceSchema() { schema_[0] = 0; }

  TransceiverDeviceSchema(const char* schema) { strncpy(schema_, schema, 16); }

  const char* raw() const { return schema_; }

  bool isDefined() const { return schema_[0] != 0; }

  // buf must have capacity of at least 16.
  void write_cstr(char* buf) const { strncpy(buf, schema_, 16); }

 private:
  char schema_[16];
};

inline bool operator==(const TransceiverDeviceSchema& a,
                       const TransceiverDeviceSchema& b) {
  return strcmp(a.raw(), b.raw()) == 0;
}

inline bool operator!=(const TransceiverDeviceSchema& a,
                       const TransceiverDeviceSchema& b) {
  return strcmp(a.raw(), b.raw()) != 0;
}

class TransceiverDeviceLocator {
 public:
  TransceiverDeviceLocator() : schema_() { device_id_[0] = 0; }

  TransceiverDeviceLocator(const char* schema, const char* device_id)
      : schema_(schema) {
    strncpy(device_id_, device_id, 24);
  }

  const TransceiverDeviceSchema& schema() const { return schema_; }
  const char* device_id() const { return device_id_; }

  bool isDefined() const { return schema_.isDefined(); }

  void write_cstr(char* buf) const {
    schema_.write_cstr(buf);
    size_t len = strlen(buf);
    if (len > 0) {
      buf[len++] = ':';
      strncpy(buf + len, device_id_, 24);
    }
  }

 private:
  TransceiverDeviceSchema schema_;
  char device_id_[24];
};

inline bool operator==(const TransceiverDeviceLocator& a,
                       const TransceiverDeviceLocator& b) {
  return a.schema() == b.schema() && strcmp(a.device_id(), b.device_id()) == 0;
}

class TransceiverSensorLocator {
 public:
  TransceiverSensorLocator() : device_locator_() { sensor_id_[0] = 0; }

  TransceiverSensorLocator(const char* schema, const char* device_id,
                           const char* sensor_id)
      : device_locator_(schema, device_id) {
    strncpy(sensor_id_, sensor_id, 24);
  }

  TransceiverSensorLocator(const TransceiverDeviceLocator& device_loc,
                           const char* sensor_id)
      : device_locator_(device_loc) {
    strncpy(sensor_id_, sensor_id, 24);
  }

  const TransceiverDeviceLocator& device_locator() const {
    return device_locator_;
  }

  const char* sensor_id() const { return sensor_id_; }

  bool isDefined() const { return device_locator().isDefined(); }

  void write_cstr(char* buf) const {
    device_locator_.write_cstr(buf);
    size_t len = strlen(buf);
    if (len > 0 && sensor_id_[0] != 0) {
      buf[len++] = '/';
      strncpy(buf + len, sensor_id_, 24);
    }
  }

 private:
  TransceiverDeviceLocator device_locator_;
  char sensor_id_[24];
};

inline bool operator==(const TransceiverSensorLocator& a,
                       const TransceiverSensorLocator& b) {
  return a.device_locator() == b.device_locator() &&
         strcmp(a.sensor_id(), b.sensor_id()) == 0;
}

class TransceiverActuatorLocator {
 public:
  TransceiverActuatorLocator() : device_locator_() { actuator_id_[0] = 0; }

  TransceiverActuatorLocator(const char* schema, const char* device_id,
                             const char* actuator_id)
      : device_locator_(schema, device_id) {
    strncpy(actuator_id_, actuator_id, 24);
  }

  const TransceiverDeviceLocator& device_locator() const {
    return device_locator_;
  }

  const char* actuator_id() const { return actuator_id_; }

  bool isDefined() const { return device_locator().isDefined(); }

  void write_cstr(char* buf) const {
    device_locator_.write_cstr(buf);
    size_t len = strlen(buf);
    if (len > 0 && actuator_id_[0] != 0) {
      buf[len++] = '/';
      strncpy(buf + len, actuator_id_, 24);
    }
  }

 private:
  TransceiverDeviceLocator device_locator_;
  char actuator_id_[24];
};

}  // namespace roo_control
