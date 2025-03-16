#include "roo_control/transceivers/remote/client.h"

#include "roo_logging.h"

#if !defined(MLOG_roo_transceivers_remote_client)
#define MLOG_roo_transceivers_remote_client 0
#endif

namespace roo_control {

TransceiverUniverseClient::TransceiverUniverseClient(
    TransceiverUniverseClientChannel& channel)
    : channel_(channel) {
  channel.registerServerMessageCallback(
      [this](const roo_control_TransceiverServerMessage& msg) {
        handleServerMessage(msg);
      });
}

TransceiverUniverseClient::~TransceiverUniverseClient() {
  channel_.registerServerMessageCallback(nullptr);
}

int TransceiverUniverseClient::deviceCount() const {
  const std::lock_guard<std::mutex> lock(state_guard_);
  return devices_.size();
}

void TransceiverUniverseClient::forEachDevice(
    std::function<bool(const TransceiverDeviceLocator&)> callback) const {
  size_t i = 0;
  TransceiverDeviceLocator loc;
  while (true) {
    {
      // Can't hold mutex for the entire iteration, b/c of deadlocks, when the
      // callback tries to call getDescriptor() or something else that grabs the
      // mutex.
      const std::lock_guard<std::mutex> lock(state_guard_);
      if (i >= devices_.size()) return;
      loc = devices_[i].locator;
    }
    if (!callback(loc)) return;
    ++i;
  }
}

const roo_control_TransceiverDescriptor*
TransceiverUniverseClient::lookupDeviceDescriptor(
    const TransceiverDeviceLocator& locator, int& descriptor_key) const {
  auto device_itr = device_idx_by_locator_.find(locator);
  if (device_itr == device_idx_by_locator_.end()) {
    // Device with the specified locator has not been found.
    return nullptr;
  }
  descriptor_key = devices_[device_itr->second].descriptor_key;
  auto descriptor_itr = descriptors_.find(descriptor_key);
  if (descriptor_itr == descriptors_.end()) {
    // Descriptor for the specified device is (erroneously) missing.
    LOG(WARNING) << "No descriptor for device " << locator;
    return nullptr;
  }
  return &descriptor_itr->second;
}

bool TransceiverUniverseClient::getDeviceDescriptor(
    const TransceiverDeviceLocator& locator,
    roo_control_TransceiverDescriptor& descriptor) const {
  const std::lock_guard<std::mutex> lock(state_guard_);
  int descriptor_key;
  const auto* result = lookupDeviceDescriptor(locator, descriptor_key);
  if (result == nullptr) return false;
  descriptor = *result;
  return true;
}

roo_control::Measurement TransceiverUniverseClient::read(
    const TransceiverSensorLocator& locator) const {
  const std::lock_guard<std::mutex> lock(state_guard_);
  int descriptor_key;
  const auto* descriptor =
      lookupDeviceDescriptor(locator.device_locator(), descriptor_key);
  if (descriptor == nullptr) return roo_control::Measurement();
  auto itr = readings_.find(locator);
  if (itr == readings_.end()) {
    return roo_control::Measurement();
  }
  return itr->second;
}

bool TransceiverUniverseClient::write(const TransceiverActuatorLocator& locator,
                                      float value) const {
  {
    const std::lock_guard<std::mutex> lock(state_guard_);

    int descriptor_key;
    const auto* descriptor =
        lookupDeviceDescriptor(locator.device_locator(), descriptor_key);
    if (descriptor == nullptr) return false;
    if (!actuators_.contains(locator)) {
      return false;
    }
  }
  {
    roo_control_TransceiverClientMessage msg =
        roo_control_TransceiverClientMessage_init_zero;
    msg.which_contents = roo_control_TransceiverClientMessage_write_tag;
    auto& payload = msg.contents.write;
    strncpy(payload.device_locator_schema, locator.schema().c_str(), 16);
    strncpy(payload.device_locator_id, locator.device_id().c_str(), 24);
    strncpy(payload.device_locator_actuator_id, locator.actuator_id().c_str(),
            24);
    payload.value = value;
    channel_.sendClientMessage(msg);
  }
  return true;
}

void TransceiverUniverseClient::requestUpdate() {
  roo_control_TransceiverClientMessage msg =
      roo_control_TransceiverClientMessage_init_zero;
  msg.which_contents = roo_control_TransceiverClientMessage_request_update_tag;
  channel_.sendClientMessage(msg);
}

void TransceiverUniverseClient::addEventListener(
    TransceiverEventListener* listener) {
  const std::lock_guard<std::mutex> lock(listener_guard_);
  listeners_.insert(listener);
}

void TransceiverUniverseClient::removeEventListener(
    TransceiverEventListener* listener) {
  const std::lock_guard<std::mutex> lock(listener_guard_);
  listeners_.erase(listener);
}

void TransceiverUniverseClient::notifyDevicesChanged() {
  const std::lock_guard<std::mutex> lock(listener_guard_);
  for (auto* listener : listeners_) {
    listener->devicesChanged();
  }
}

void TransceiverUniverseClient::notifyReadingsAvailable() {
  const std::lock_guard<std::mutex> lock(listener_guard_);
  for (auto* listener : listeners_) {
    listener->newReadingsAvailable();
  }
}

void TransceiverUniverseClient::handleServerMessage(
    const roo_control_TransceiverServerMessage& msg) {
  switch (msg.which_contents) {
    case roo_control_TransceiverServerMessage_transceiver_update_begin_tag: {
      if (!msg.contents.transceiver_update_begin.delta) {
        clearAll();
      }
      CHECK(updated_devices_.empty());
      break;
    }
    case roo_control_TransceiverServerMessage_descriptor_added_tag: {
      handleDescriptorAdded(msg.contents.descriptor_added.key,
                            msg.contents.descriptor_added.descriptor);
      break;
    }
    case roo_control_TransceiverServerMessage_descriptor_removed_tag: {
      handleDescriptorRemoved(msg.contents.descriptor_removed.key);
      break;
    }
    case roo_control_TransceiverServerMessage_device_added_tag: {
      roo_control::TransceiverDeviceLocator loc(
          msg.contents.device_added.locator_schema,
          msg.contents.device_added.locator_id);
      handleDeviceAdded(loc, msg.contents.device_added.descriptor_key);
      break;
    }
    case roo_control_TransceiverServerMessage_device_removed_tag: {
      handleDeviceRemoved(msg.contents.device_removed.prev_index);
      break;
    }
    case roo_control_TransceiverServerMessage_device_preserved_tag: {
      size_t count = 1;
      const auto& payload = msg.contents.device_preserved;
      if (payload.has_count) {
        count = payload.count;
      }
      handleDevicePreserved(payload.prev_index, count);
      break;
    }
    case roo_control_TransceiverServerMessage_device_modified_tag: {
      handleDeviceModified(msg.contents.device_modified.prev_index,
                           msg.contents.device_modified.descriptor_key);
      break;
    }
    case roo_control_TransceiverServerMessage_transceiver_update_end_tag: {
      handleTransceiverUpdateEnd();
      break;
    }
    case roo_control_TransceiverServerMessage_readings_begin_tag: {
      handleReadingsBegin();
      break;
    }
    case roo_control_TransceiverServerMessage_reading_tag: {
      auto& payload = msg.contents.reading;
      roo_control::TransceiverDeviceLocator device(
          payload.device_locator_schema, payload.device_locator_id);
      handleReadings(device, payload.sensor_values,
                     payload.sensor_values_count);
      break;
    }
    case roo_control_TransceiverServerMessage_readings_end_tag: {
      handleReadingsEnd();
      break;
    }

    default: {
      LOG(ERROR) << "Unexpected server message " << msg.which_contents;
      break;
    }
  }
}

void TransceiverUniverseClient::handleTransceiverUpdateEnd() {
  {
    std::lock_guard<std::mutex> lock(state_guard_);
    devices_.swap(updated_devices_);
    updated_devices_.clear();
    device_idx_by_locator_.clear();
    for (size_t i = 0; i < devices_.size(); ++i) {
      device_idx_by_locator_[devices_[i].locator] = i;
    }
  }
  notifyDevicesChanged();
}

void TransceiverUniverseClient::handleDescriptorAdded(
    int key, const roo_control_TransceiverDescriptor& descriptor) {
  MLOG(roo_transceivers_remote_client) << "Received added descriptor";
  const std::lock_guard<std::mutex> lock(state_guard_);
  descriptors_[key] = descriptor;
}

void TransceiverUniverseClient::handleDescriptorRemoved(int key) {
  MLOG(roo_transceivers_remote_client) << "Received removed descriptor";
  const std::lock_guard<std::mutex> lock(state_guard_);
  // At this point we do not expect to have any devices pointing to this
  // descriptor.
  descriptors_.erase(key);
}

void TransceiverUniverseClient::handleDeviceAdded(
    const roo_control::TransceiverDeviceLocator& locator, int descriptor_key) {
  MLOG(roo_transceivers_remote_client) << "Received added device " << locator;
  const std::lock_guard<std::mutex> lock(state_guard_);
  auto itr = descriptors_.find(descriptor_key);
  if (itr == descriptors_.end()) {
    LOG(ERROR) << "Unknown descriptor key " << descriptor_key;
    return;
  }
  updated_devices_.push_back(DeviceEntry{locator, descriptor_key});
  const roo_control_TransceiverDescriptor& descriptor = itr->second;
  // Pre-initialize all sensor readings to set quantities.
  for (size_t i = 0; i < descriptor.sensors_count; ++i) {
    TransceiverSensorLocator sensor_locator(locator, descriptor.sensors[i].id);
    readings_[sensor_locator] =
        Measurement(descriptor.sensors[i].quantity, roo_time::Uptime::Start());
  }
  // Also, register the actuators for fast lookup during write().
  for (size_t i = 0; i < descriptor.actuators_count; ++i) {
    TransceiverActuatorLocator actuator_locator(locator,
                                                descriptor.actuators[i].id);
    actuators_.insert(actuator_locator);
  }
}

void TransceiverUniverseClient::handleDeviceRemoved(int prev_index) {
  // Erase all readings.
  int descriptor_key;
  const TransceiverDeviceLocator& locator = devices_[prev_index].locator;
  MLOG(roo_transceivers_remote_client) << "Received removed device " << locator;
  const roo_control_TransceiverDescriptor* descriptor =
      lookupDeviceDescriptor(locator, descriptor_key);
  if (descriptor == nullptr) {
    LOG(ERROR) << "Missing device descriptor for " << locator;
  } else {
    for (size_t i = 0; i < descriptor->sensors_count; ++i) {
      TransceiverSensorLocator sensor_locator(locator,
                                              descriptor->sensors[i].id);
      readings_.erase(sensor_locator);
    }
    for (size_t i = 0; i < descriptor->actuators_count; ++i) {
      TransceiverActuatorLocator actuator_locator(locator,
                                                  descriptor->actuators[i].id);
      actuators_.erase(actuator_locator);
    }
  }
  // That's it - we're not adding anything to updated_devices_, and
  // device_idx_by_locator will be refreshed on transceiver_update_end.
}

void TransceiverUniverseClient::handleDevicePreserved(int prev_index_first,
                                                      size_t count) {
  const std::lock_guard<std::mutex> lock(state_guard_);
  MLOG(roo_transceivers_remote_client)
      << "Received preserved devices (" << count << " at " << prev_index_first
      << ")";
  for (size_t i = 0; i < count; ++i) {
    updated_devices_.push_back(devices_[prev_index_first + i]);
  }
}

void TransceiverUniverseClient::handleDeviceModified(int prev_index,
                                                     int descriptor_key) {
  MLOG(roo_transceivers_remote_client)
      << "Received modified device at " << prev_index;
  const std::lock_guard<std::mutex> lock(state_guard_);
  updated_devices_.push_back(
      DeviceEntry{devices_[prev_index].locator, descriptor_key});
}

void TransceiverUniverseClient::clearAll() {
  const std::lock_guard<std::mutex> lock(state_guard_);
  descriptors_.clear();
  devices_.clear();
  updated_devices_.clear();
  device_idx_by_locator_.clear();
  readings_.clear();
  actuators_.clear();
}

void TransceiverUniverseClient::handleReadings(
    const roo_control::TransceiverDeviceLocator& device,
    const roo_control_TransceiverServerMessage_Reading_SensorValue* readings,
    size_t readings_count) {
  const std::lock_guard<std::mutex> lock(state_guard_);
  int descriptor_key;
  const roo_control_TransceiverDescriptor* descriptor =
      lookupDeviceDescriptor(device, descriptor_key);
  if (descriptor == nullptr) {
    LOG(ERROR) << "Missing device descriptor for " << device;
    return;
  }
  roo_time::Uptime now = roo_time::Uptime::Now();
  for (size_t i = 0; i < readings_count; ++i) {
    TransceiverSensorLocator sensor_locator(
        device, readings[i].device_locator_sensor_id);
    roo_control::Measurement& m = readings_[sensor_locator];
    // Overwrite the measurement with new value and time (but keep the
    // quantity).
    MLOG(roo_transceivers_remote_client)
        << "Received reading of " << sensor_locator << ": " << m.value();
    m = roo_control::Measurement(m.quantity(),
                                 now - roo_time::Millis(readings[i].age_ms),
                                 readings[i].value);
  }
}

void TransceiverUniverseClient::handleReadingsBegin() {}

void TransceiverUniverseClient::handleReadingsEnd() {
  notifyReadingsAvailable();
}

}  // namespace roo_control