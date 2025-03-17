#include "roo_control/transceivers/binding/hal/arduino_prefs/arduino_prefs_store.h"

#include <cstdio>

#include "roo_backport.h"

namespace roo_control {

namespace {

void id2key(int id, char* key) { sprintf(key, "r_%d", id); }

}  // namespace

using Rep = char[64];

TransceiverSensorLocator
ArduinoPreferencesTransceiverBindingStore::getSensorBinding(SensorKey key) {
  roo_prefs::Transaction t(collection_, true);
  char skey[16];
  id2key(key, skey);
  Rep rep;
  roo_prefs::ReadResult result = t.store().readObject(skey, rep);
  if (result != roo_prefs::READ_OK) {
    return TransceiverSensorLocator();
  }
  return TransceiverSensorLocator(roo::string_view(&rep[0], 16),
                                  roo::string_view(&rep[16], 24),
                                  roo::string_view(&rep[40], 24));
}

void ArduinoPreferencesTransceiverBindingStore::setSensorBinding(
    SensorKey key, const TransceiverSensorLocator& locator) {
  roo_prefs::Transaction t(collection_);
  char skey[16];
  id2key(key, skey);
  Rep rep;
  memset(&rep[0], 0, 64);
  strncpy(&rep[0], locator.schema().c_str(), 16);
  strncpy(&rep[16], locator.device_id().c_str(), 24);
  strncpy(&rep[40], locator.sensor_id().c_str(), 24);
  t.store().writeObject(skey, rep);
}

void ArduinoPreferencesTransceiverBindingStore::clearSensorBinding(
    SensorKey key) {
  roo_prefs::Transaction t(collection_);
  char skey[16];
  id2key(key, skey);
  t.store().clear(skey);
}

}  // namespace roo_control
