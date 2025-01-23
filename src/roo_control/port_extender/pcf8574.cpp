#include "roo_control/port_extender/pcf8574.h"

namespace roo_control {

using roo_time::Interval;
using roo_time::Millis;
using roo_time::Uptime;

bool Pcf8574::Port::setState(BinaryLogicalState state) {
  return extender_.writePort(port_, state);
}

BinaryLogicalState Pcf8574::Port::getState() const {
  return extender_.readPort(port_);
}

Pcf8574::Port::Port(Pcf8574& extender, uint8_t port)
    : extender_(extender), port_(port) {}

bool Pcf8574::OutputPort::setState(BinaryLogicalState state) {
  return extender_.writePort(port_, state);
}

BinaryLogicalState Pcf8574::OutputPort::getState() const {
  return ((extender_.last_written_ & (1 << port_)) != 0) ? BINARY_STATE_HIGH
                                                         : BINARY_STATE_LOW;
}

Pcf8574::OutputPort::OutputPort(Pcf8574& extender, uint8_t port)
    : extender_(extender), port_(port) {}

Pcf8574::Pcf8574(TwoWire& wire, uint8_t addr)
    : wire_(wire),
      address_(addr),
      last_written_(0xFF),
      last_read_(0xFF),
      last_read_time_(Uptime::Start()),
      last_read_cache_duration_(Millis(20)) {}


  // // Reads the level of the specified port. For ports used as output, the value
  // // will reflect what was last written.
  // // On a communication failure, returns the last known state.
  // BinaryLogicalState readPort(uint8_t port);

  // // Writes the level of the specified output port. Returns false on a
  // // communication failure. For input ports, it is OK to write HIGH - that will
  // // allow the slave to continue driving the level actaully seen by readPort.
  // // Writing LOW will force the LOW state.
  // bool writePort(uint8_t port, BinaryLogicalState state);


BinaryLogicalState Pcf8574::readPort(uint8_t port) {
  uint8_t data;
  if (!read(data)) {
    data = last_read();
  }
  uint8_t mask = (1 << port);
  return ((data & mask) != 0) ? BINARY_STATE_HIGH : BINARY_STATE_LOW;
}

bool Pcf8574::writePort(uint8_t port, BinaryLogicalState state) {
  if (state == BINARY_STATE_HIGH) {
    uint8_t mask = (1 << port);
    if ((last_written_ & mask) != 0) {
      // No change since last write.
      return true;
    }
    return write(last_written_ | mask);
  } else {
    uint8_t mask = ~(1 << port);
    if ((last_written_ & mask) == 0) {
      // No change since last write.
      return true;
    }
    return write(last_written_ & mask);
  }
}

bool Pcf8574::read(uint8_t& data) {
  Uptime now = Uptime::Now();
  if (last_read_time_ + last_read_cache_duration_ > now &&
      last_read_time_ > Uptime::Start()) {
    // Cached value is valid; return it.
    data = last_read_;
    return true;
  }
  wire_.requestFrom(address_, (uint8_t)1);
  if (!wire_.available()) {
    LOG(ERROR) << "I2C read error";
    return false;
  }
  last_read_ = wire_.read();
  last_read_time_ = now;
  data = last_read_;
  return true;
}

bool Pcf8574::write(uint8_t data) {
  wire_.beginTransmission(address_);
  wire_.write(data);
  if (wire_.endTransmission() != 0) {
    LOG(ERROR) << "I2C write error";
    return false;
  }
  last_written_ = data;
  // Invalidate the read cache, since the change of external state is likely
  // to have an effect also on the inputs.
  last_read_time_ = Uptime::Start();
  // Still, update the cached value to incorporate zeroed bits, in case it is
  // returned due to I2C read failure.
  last_read_ &= last_written_;
  return true;
}

}  // namespace roo_control