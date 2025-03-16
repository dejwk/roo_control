#include "roo_control/transceivers/id.h"

namespace roo_control {

TransceiverDeviceLocator::TransceiverDeviceLocator()
    : schema_(), device_id_() {}

TransceiverDeviceLocator::TransceiverDeviceLocator(roo::string_view schema,
                                                   roo::string_view device_id)
    : schema_(schema), device_id_(device_id) {}

void TransceiverDeviceLocator::write_cstr(char* buf) const {
  strncpy(buf, schema_.c_str(), TransceiverDeviceSchema::kCapacity);
  size_t len = strlen(buf);
  if (len > 0) {
    buf[len++] = ':';
    strncpy(buf + len, device_id_.c_str(), 24);
  }
}

TransceiverSensorLocator::TransceiverSensorLocator()
    : device_locator_(), sensor_id_() {}

TransceiverSensorLocator::TransceiverSensorLocator(roo::string_view schema,
                                                   roo::string_view device_id,
                                                   roo::string_view sensor_id)
    : device_locator_(schema, device_id), sensor_id_(sensor_id) {}

TransceiverSensorLocator::TransceiverSensorLocator(
    const TransceiverDeviceLocator& device_loc, roo::string_view sensor_id)
    : device_locator_(device_loc), sensor_id_(sensor_id) {}

void TransceiverSensorLocator::write_cstr(char* buf) const {
  device_locator_.write_cstr(buf);
  size_t len = strlen(buf);
  if (len > 0 && !sensor_id_.empty()) {
    buf[len++] = '/';
    strncpy(buf + len, sensor_id_.c_str(), 24);
  }
}

TransceiverActuatorLocator::TransceiverActuatorLocator()
    : device_locator_(), actuator_id_() {}

TransceiverActuatorLocator::TransceiverActuatorLocator(
    const TransceiverDeviceLocator& device_loc, roo::string_view actuator_id)
    : device_locator_(device_loc), actuator_id_(actuator_id) {}

TransceiverActuatorLocator::TransceiverActuatorLocator(
    roo::string_view schema, roo::string_view device_id,
    roo::string_view actuator_id)
    : device_locator_(schema, device_id), actuator_id_(actuator_id) {}

void TransceiverActuatorLocator::write_cstr(char* buf) const {
  device_locator_.write_cstr(buf);
  size_t len = strlen(buf);
  if (len > 0 && !actuator_id_.empty()) {
    buf[len++] = '/';
    strncpy(buf + len, actuator_id_.c_str(), 24);
  }
}

}  // namespace roo_control

roo_logging::Stream& operator<<(
    roo_logging::Stream& s,
    const roo_control::TransceiverDeviceSchema& schema) {
  s << schema.c_str();
  return s;
}

roo_logging::Stream& operator<<(
    roo_logging::Stream& s, const roo_control::TransceiverDeviceLocator& loc) {
  s << loc.schema() << ":" << loc.device_id().c_str();
  return s;
}

roo_logging::Stream& operator<<(
    roo_logging::Stream& s, const roo_control::TransceiverSensorLocator& loc) {
  s << loc.device_locator();
  if (!loc.sensor_id().empty()) {
    s << "/" << loc.sensor_id().c_str();
  }
  return s;
}

roo_logging::Stream& operator<<(
    roo_logging::Stream& s,
    const roo_control::TransceiverActuatorLocator& loc) {
  s << loc.device_locator();
  if (!loc.actuator_id().empty()) {
    s << "/" << loc.actuator_id().c_str();
  }
  return s;
}