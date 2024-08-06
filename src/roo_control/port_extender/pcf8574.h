#pragma once

#include "Wire.h"
#include "roo_control/switch.h"
#include "roo_time.h"

namespace roo_control {

// PCF8574 (https://www.ti.com/lit/ds/symlink/pcf8574.pdf) is an I2C-controlled
// 8 port extender. Each port can be used as digital input, digital output, or
// quasi-bidirectional.
//
// To use a given port as output, simply drive the corresponding bit from the
// extender, and read it from the slave. See the datasheet, above, for details.
//
// To use a given port as input, set the corresponding bit to HIGH (which is the
// initial value), and then use the slave to drive the line to VCC or GND.
class Pcf8574 {
 public:
  // Represents a quasi-bidirectional port. Can be used as borth input and
  // output.
  // If your port is output-only, prefer `OutputPort` instead.
  class Port : public BinarySwitch {
   public:
    Port(Pcf8574& extender, uint8_t port);

    bool setState(BinarySwitchState state) override;
    BinarySwitchState getState() const override;

   private:
    Pcf8574& extender_;
    const uint8_t port_;
  };

  // Represents an output-only port. Reading the state of the port (by calling
  // getState()) always returns the last known set value, and never has to
  // communicate with the extender.
  //
  // Prefer this to `Port` for output-only ports.
  class OutputPort : public BinarySwitch {
   public:
    OutputPort(Pcf8574& extender, uint8_t port);

    bool setState(BinarySwitchState state) override;
    BinarySwitchState getState() const override;

   private:
    Pcf8574& extender_;
    const uint8_t port_;
  };

  // Creates the extender connected to the specified two-wire bus, and using the
  // specified I2C address.
  Pcf8574(TwoWire& wire, uint8_t addr);

  // Returns the maximum allowed staleness of data read from the inputs. Useful
  // to reduce the amount of unneccessary repetitive I2C reads. Defaults to
  // 20ms.
  roo_time::Interval getReadCacheDuration() const {
    return last_read_cache_duration_;
  }

  // Sets the maximum allowed staleness of data read from the inputs.
  void setReadCacheDuration(roo_time::Interval duration) {
    last_read_cache_duration_ = duration;
  }

  // Reads, caches, and returns the actual levels of all ports. If called within
  // the 'read cache duration' interval from the last read, may return cached
  // results. Returns false on a communication failure.
  bool read(uint8_t& data);

  // Writes the levels of all ports. For ports used as input, the corresponding
  // bits should be set HIGH, in order to continue allowing the slave to drive
  // the actual line level. Returns false on a communication failure.
  bool write(uint8_t data);

  // Returns the byte that was the most recently read. If the data was never
  // read, returns 0xFF.
  uint8_t last_read() const { return last_read_; }

  // Returns the byte that was the most recently written. If the data was never
  // written, returnx 0xFF.
  uint8_t last_written() const { return last_written_; }

  // Reads the level of the specified port. For ports used as output, the value
  // will generally reflect what was last written, but the value is always actually read
  // from the extender. On a communication failure, returns the last known
  // state.
  BinarySwitchState readPort(uint8_t port);

  // Writes the level of the specified output port. Returns false on a
  // communication failure. For input ports, it is OK to write HIGH - that will
  // allow the slave to continue driving the level actaully seen by readPort.
  // Writing LOW will force the LOW state.
  bool writePort(uint8_t port, BinarySwitchState state);

 private:
  // The I2C interface.
  TwoWire& wire_;

  // The I2C device address.
  uint8_t address_;

  // The byte that was last written (initially 0xFF).
  uint8_t last_written_;

  // The byte that was last read (initially 0xFF).
  uint8_t last_read_;

  // The time of last read, or Uptime::Start if never read or if the cached
  // value (last_read_) is considered invalid.
  roo_time::Uptime last_read_time_;

  // Maximum staleness of values returned by read().
  roo_time::Interval last_read_cache_duration_;
};

}  // namespace roo_control