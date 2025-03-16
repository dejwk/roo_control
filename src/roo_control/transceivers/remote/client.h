#pragma once

#include <functional>
#include <mutex>
#include <vector>

#include "roo_collections/flat_small_hash_map.h"
#include "roo_collections/flat_small_hash_set.h"
#include "roo_control/transceivers/id.h"
#include "roo_control/transceivers/notification.h"
#include "roo_control/transceivers/universe.h"

namespace roo_control {

class TransceiverUniverseClientChannel {
 public:
  using ServerMessageCb =
      std::function<void(const roo_control_TransceiverServerMessage&)>;

  virtual ~TransceiverUniverseClientChannel() = default;

  virtual void registerServerMessageCallback(ServerMessageCb cb) = 0;

  virtual void sendClientMessage(
      const roo_control_TransceiverClientMessage& msg) = 0;
};

// The universe that mirrors a remote universe, with with it synchronizes the
// state using a bi-directional serial communication channel.
class TransceiverUniverseClient : public TransceiverUniverse {
 public:
  struct DeviceEntry {
    roo_control::TransceiverDeviceLocator locator;
    int descriptor_key;
  };

  TransceiverUniverseClient(TransceiverUniverseClientChannel& channel);

  ~TransceiverUniverseClient();

  int deviceCount() const override;

  void forEachDevice(std::function<bool(const TransceiverDeviceLocator&)>
                         callback) const override;

  bool getDeviceDescriptor(
      const TransceiverDeviceLocator& locator,
      roo_control_TransceiverDescriptor& descriptor) const override;

  Measurement read(const TransceiverSensorLocator& locator) const override;

  bool write(const TransceiverActuatorLocator& locator,
             float value) const override;

  void requestUpdate() override;

  void addEventListener(TransceiverEventListener* listener) override;

  void removeEventListener(TransceiverEventListener* listener) override;

 private:
  // Convenience function to lookup a descriptor by device locator. Additionally
  // returns the descriptor key, assigned by the server.
  const roo_control_TransceiverDescriptor* lookupDeviceDescriptor(
      const TransceiverDeviceLocator& locator, int& descriptor_key) const;

  // Called when a message is received from the server.
  void handleServerMessage(const roo_control_TransceiverServerMessage& msg);

  void handleDescriptorAdded(
      int key, const roo_control_TransceiverDescriptor& descriptor);

  void handleDescriptorRemoved(int key);

  void handleDevice(const roo_control::TransceiverDeviceLocator& locator,
                    int descriptor_key);

  void handleDeviceAdded(const roo_control::TransceiverDeviceLocator& locator,
                         int descriptor_key);

  void handleDeviceRemoved(int prev_index);

  void handleDevicePreserved(int prev_index_first, size_t count);

  void handleDeviceModified(int prev_index, int descriptor_key);

  // Helper method to propagate the devices event to listeners.
  void notifyDevicesChanged();

  // Helper method to propagate the readings event to listeners.
  void notifyReadingsAvailable();

  void clearAll();

  void handleTransceiverUpdateEnd();

  void handleReadingsBegin();

  void handleReadings(
      const roo_control::TransceiverDeviceLocator& device,
      const roo_control_TransceiverServerMessage_Reading_SensorValue* readings,
      size_t readings_count);

  void handleReadingsEnd();

  // The channel used to synchronize the state (i.e., receive measurements from
  // and push writes to) with the remote universe.
  TransceiverUniverseClientChannel& channel_;

  // Stores all descriptors for devices handled by the remote universe. The
  // descriptors are assigned integer keys by the remote universe. These are
  // used as keys into this map.
  roo_collections::FlatSmallHashMap<int, roo_control_TransceiverDescriptor>
      descriptors_;

  // For each device known to the remote universe, points to the key of
  // the descriptor for that device. That key can be used to look up the
  // descriptor in the descriptors_ map.
  roo_collections::FlatSmallHashMap<TransceiverDeviceLocator, int>
      device_idx_by_locator_;

  std::vector<DeviceEntry> devices_;

  std::vector<DeviceEntry> updated_devices_;

  // Cached sensor measurements.
  roo_collections::FlatSmallHashMap<TransceiverSensorLocator,
                                    roo_control::Measurement>
      readings_;

  // Actuators by actuator ID per device type. Allows us to quickly verify that
  // an actuator locator used for write actually does exist, before sending the
  // write request to the server.
  roo_collections::FlatSmallHashSet<TransceiverActuatorLocator> actuators_;

  // Event listeners for state updates.
  roo_collections::FlatSmallHashSet<TransceiverEventListener*> listeners_;

  mutable std::mutex state_guard_;
  mutable std::mutex listener_guard_;
};

}  // namespace roo_control