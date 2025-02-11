#include "roo_control/transceivers/binding/hal/arduino_prefs/arduino_prefs_store.h"

#include <cstdio>

namespace roo_control {

namespace {

void id2key(int id, char* key) { sprintf(key, "r_%d", id); }

}  // namespace

TransceiverSensorLocator
ArduinoPreferencesTransceiverBindingStore::getSensorBinding(SensorKey key) {
  roo_prefs::Transaction t(collection_, true);
  char skey[16];
  id2key(key, skey);
  TransceiverSensorLocator loc;
  roo_prefs::ReadResult result =
      roo_prefs::StoreRead<TransceiverSensorLocator>(t.store(), skey, loc);
  return (result == roo_prefs::READ_OK) ? loc : TransceiverSensorLocator();
}

void ArduinoPreferencesTransceiverBindingStore::setSensorBinding(
    SensorKey key, const TransceiverSensorLocator& locator) {
  roo_prefs::Transaction t(collection_);
  char skey[16];
  id2key(key, skey);
  roo_prefs::StoreWrite<TransceiverSensorLocator>(t.store(), skey, locator);
}

void ArduinoPreferencesTransceiverBindingStore::clearSensorBinding(
    SensorKey key) {
  roo_prefs::Transaction t(collection_);
  char skey[16];
  id2key(key, skey);
  roo_prefs::StoreClear(t.store(), skey);
}

}  // namespace roo_control
