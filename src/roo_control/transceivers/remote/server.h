#pragma once

#include <functional>
#include <mutex>
#include <vector>

#include "roo_control/transceivers/notification.h"
#include "roo_control/transceivers/universe.h"

namespace roo_control {

class Executor {
 public:
  virtual ~Executor() = default;

  virtual void execute(std::function<void()> task) = 0;
};

struct TransceiverDescriptorHashFn {
  size_t operator()(const roo_control_TransceiverDescriptor& descriptor) const;
};

struct TransceiverDescriptorEq {
  bool operator()(const roo_control_TransceiverDescriptor& a,
                  const roo_control_TransceiverDescriptor& b) const;
};

struct DescriptorEntry {
  int key;
  int refcount;
};

using TransceiverDescriptorMap = roo_collections::FlatSmallHashMap<
    roo_control_TransceiverDescriptor, DescriptorEntry,
    TransceiverDescriptorHashFn, TransceiverDescriptorEq>;

class TransceiverUniverseServerChannel {
 public:
  using ClientMessageCb =
      std::function<void(const roo_control_TransceiverClientMessage&)>;

  virtual ~TransceiverUniverseServerChannel() = default;

  virtual void registerClientMessageCallback(ClientMessageCb cb) = 0;

  virtual void sendServerMessage(
      const roo_control_TransceiverServerMessage& msg) = 0;
};

// The server keeps the cached state of the underlying universe, along with the
// most recent delta. It also has state, which indicates whether a transmission
// is currently in progress, and whether the universe state has changed since
// the last snapshot was taken. At any given time, server is either not
// transmitting (when there are no deltas to transmit), or it is transmitting
// the most recent delta.
//
// Starting the transmission is always preceded with taking a new snapshot,
// calculating the delta relative to the previous snapshot, and clearing the
// state to 'unchanged'. The new snapshot and the delta then remains unchanged
// during transmission.
//
// Upon receiving the change notification, if the server is currently
// transmitting, the state is updated to indicate that the universe has changed,
// but no other action is taken. If the transmission is not in progress, the
// 'snapshot-delta-transmit' process is triggered.
//
// Upon finishing the transfer, if 'change' state is 'changed', the
// 'snapshot-delta-transmit' is immediately triggered again (that is, a new
// delta is calculated and a new transfer is immediately started). Otherwise,
// the 'transmit' state is changed to 'not transmitting'.
//
// All state changes are guarded by a mutex, to synchronize transmits with
// 'change' events.
class TransceiverUniverseServer : public TransceiverEventListener {
 public:
  TransceiverUniverseServer(TransceiverUniverse& universe,
                            TransceiverUniverseServerChannel& channel,
                            Executor& transmit_executor);

  ~TransceiverUniverseServer();

  void devicesChanged() override;
  void newReadingsAvailable() override;

 private:
  class State {
   public:
    size_t device_count() const { return devices_.size(); }

    void addDevice(const roo_control::TransceiverDeviceLocator& loc,
                   const roo_control_TransceiverDescriptor& descriptor,
                   int ordinal) {
      newDeviceDelta(loc, DeviceDelta::ADDED);
      int key = addDescriptorReference(descriptor);
      addDeviceEntry(loc, ordinal, key);
    }

    void removeReadings(const roo_control::TransceiverDeviceLocator& loc,
                        const roo_control_TransceiverDescriptor& descriptor) {
      for (size_t i = 0; i < descriptor.sensors_count; ++i) {
        eraseSensorReading(
            TransceiverSensorLocator(loc, descriptor.sensors[i].id));
      }
    }

    void removeDevice(const roo_control::TransceiverDeviceLocator& loc) {
      newDeviceDelta(loc, State::DeviceDelta::REMOVED);
      int old_descriptor_key = devices_[loc].descriptor_key;
      const roo_control_TransceiverDescriptor& old_descriptor =
          descriptors_by_key_[old_descriptor_key];
      removeReadings(loc, old_descriptor);
      removeDescriptorReference(old_descriptor);
      devices_.erase(loc);
    }

    int addDescriptorReference(
        const roo_control_TransceiverDescriptor& descriptor);

    void addDeviceEntry(const roo_control::TransceiverDeviceLocator& loc,
                        int ordinal, int descriptor_key) {
      devices_[loc] = DeviceEntry{ordinal, descriptor_key};
    }

    void removeDescriptorReference(
        const roo_control_TransceiverDescriptor& descriptor);

    void eraseSensorReading(const TransceiverSensorLocator& loc) {
      readings_.erase(loc);
    }

    bool updateSensorReading(const TransceiverSensorLocator& loc,
                             const roo_control::Measurement& m) {
      auto itr = readings_.find(loc);
      if (itr == readings_.end()) {
        // Did not have prior measurement.
        if (m.isInitial()) return false;
        readings_[loc] = SensorReading{m.value(), m.time()};
      } else {
        // Did have prior measurement.
        auto& v = itr->second;
        if (((m.value() == v.value) ||
             (isnanf(m.value()) && isnanf(v.value))) &&
            (m.time() == v.time)) {
          // The value did not change.
          return false;
        }
        // The value did change. Update, and write to the delta.
        if (m.isInitial()) {
          eraseSensorReading(loc);
        } else {
          v.value = m.value();
          v.time = m.time();
        }
      }
      return true;
    }

    const roo_control_TransceiverDescriptor& getDescriptor(
        const roo_control::TransceiverDeviceLocator& loc) const {
      return descriptors_by_key_[devices_[loc].descriptor_key];
    }

    void clearAll();
    void clearDelta();

    struct DeviceEntry {
      int ordinal;  // the index in the universe.
      int descriptor_key;
    };

    struct DeviceDelta {
      enum Status { ADDED, REMOVED, PRESERVED, MODIFIED };
      TransceiverDeviceLocator locator;
      Status status;
    };

    struct DescriptorDelta {
      enum Status { ADDED, REMOVED };
      int key;
      Status status;
    };

    struct SensorReadingDelta {
      TransceiverSensorId sensor_id;
      float value;
      roo_time::Uptime time;
    };

    struct SensorReadingDeltaDeviceGroup {
      TransceiverDeviceLocator device;
      size_t reading_count;
    };

    void newDeviceDelta(const roo_control::TransceiverDeviceLocator& loc,
                        DeviceDelta::Status status) {
      device_deltas_.emplace_back(DeviceDelta{loc, status});
    }

    void newDescriptorDelta(int key, DescriptorDelta::Status kind) {
      descriptor_deltas_.emplace_back(DescriptorDelta{key, kind});
    }

    void newSensorReadingDelta(const roo_control::TransceiverSensorLocator& loc,
                               float value, roo_time::Uptime time);

    const std::vector<DeviceDelta>& device_deltas() const {
      return device_deltas_;
    }

    const std::vector<DescriptorDelta>& descriptor_deltas() const {
      return descriptor_deltas_;
    }

    const roo_collections::FlatSmallHashMap<TransceiverDeviceLocator,
                                            DeviceEntry>&
    devices() const {
      return devices_;
    }

    const roo_collections::FlatSmallHashMap<int,
                                            roo_control_TransceiverDescriptor>&
    descriptors_by_key() const {
      return descriptors_by_key_;
    }

    const std::vector<SensorReadingDeltaDeviceGroup>& reading_delta_groups()
        const {
      return reading_delta_groups_;
    }

    const std::vector<SensorReadingDelta>& reading_deltas() const {
      return reading_deltas_;
    }

   private:
    struct SensorReading {
      float value;
      roo_time::Uptime time;
    };

    roo_collections::FlatSmallHashMap<TransceiverDeviceLocator, DeviceEntry>
        devices_;

    TransceiverDescriptorMap descriptors_;

    roo_collections::FlatSmallHashMap<int, roo_control_TransceiverDescriptor>
        descriptors_by_key_;

    roo_collections::FlatSmallHashMap<TransceiverSensorLocator, SensorReading>
        readings_;

    std::vector<DeviceDelta> device_deltas_;
    std::vector<DescriptorDelta> descriptor_deltas_;

    // Devices appear in the order of enumeration by the universe.
    std::vector<SensorReadingDeltaDeviceGroup> reading_delta_groups_;
    std::vector<SensorReadingDelta> reading_deltas_;
  };

  void handleClientMessage(const roo_control_TransceiverClientMessage& msg);

  void handleRequestState();

  void triggerTransmission();

  void snapshotDevices();
  void snapshotSensorState();

  void transmissionLoop();

  // Sends a single delta or snapshot over the channel.
  void transmit(bool is_delta);

  void transmitUpdateBegin(bool delta);
  void transmitUpdateEnd();

  void transmitDescriptorAdded(int key);
  void transmitDescriptorRemoved(int key);

  void transmitDeviceAdded(const TransceiverDeviceLocator& locator,
                           int descriptor_key);
  void transmitDevicesPreserved(int first_preserved_ordinal, size_t count);
  void transmitDeviceModified(int prev_ordinal, int descriptor_key);
  void transmitDeviceRemoved(int prev_ordinal);

  void transmitReadingsBegin();
  void transmitReadingsEnd();

  TransceiverUniverse& universe_;
  TransceiverUniverseServerChannel& channel_;
  Executor& transmit_executor_;

  State state_;

  bool full_snapshot_requested_;
  bool is_full_snapshot_;
  bool transmission_in_progress_;
  bool device_update_pending_;
  bool readings_pending_;

  mutable std::mutex state_guard_;
};

}  // namespace roo_control