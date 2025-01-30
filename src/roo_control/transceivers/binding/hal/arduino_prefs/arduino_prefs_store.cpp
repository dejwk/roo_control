#include "roo_control/transceivers/binding/hal/arduino_prefs/arduino_prefs_store.h"

#include <cstdio>

namespace roo_control {

namespace {

void id2key(int id, char* key) { sprintf(key, "r_%d", id); }

}  // namespace

UniversalTransceiverDeviceId ArduinoPreferencesSensorBindingStore::getBinding(Key key) {
  roo_prefs::Transaction t(collection_, true);
  char skey[16];
  id2key(key, skey);
  UniversalTransceiverDeviceId id;
  roo_prefs::ReadResult result =
      roo_prefs::StoreRead<UniversalTransceiverDeviceId>(t.store(), skey, id);
  return (result == roo_prefs::READ_OK ? id : UniversalTransceiverDeviceId());
}

void ArduinoPreferencesSensorBindingStore::setBinding(Key key,
                                                      UniversalTransceiverDeviceId id) {
  roo_prefs::Transaction t(collection_);
  char skey[16];
  id2key(key, skey);
  roo_prefs::StoreWrite<UniversalTransceiverDeviceId>(t.store(), skey, id);
}

void ArduinoPreferencesSensorBindingStore::clearBinding(Key key) {
  roo_prefs::Transaction t(collection_);
  char skey[16];
  id2key(key, skey);
  roo_prefs::StoreClear(t.store(), skey);
}

}  // namespace roo_control
