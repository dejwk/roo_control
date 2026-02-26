#pragma once

#include "Wire.h"
#include "roo_control/switch/switch.h"
#include "roo_time.h"

namespace roo_control {

/// PCF8574 I2C-controlled 8-bit port extender.
///
/// Datasheet: https://www.ti.com/lit/ds/symlink/pcf8574.pdf
///
/// Each port can be used as digital input, digital output, or
/// quasi-bidirectional.
///
/// To use a given port as output, drive the corresponding bit from the
/// extender, and read it from the slave. See the datasheet for details.
///
/// To use a given port as input, set the corresponding bit to HIGH (the
/// initial value), and then use the slave to drive the line to VCC or GND.
class Pcf8574 {
 public:
  /// Quasi-bidirectional port usable as both input and output.
  ///
  /// If your port is output-only, prefer `OutputPort` instead.
  class Port : public BinarySwitch {
   public:
    Port(Pcf8574& extender, uint8_t port);

    bool setState(BinaryLogicalState state) override;
    bool getState(BinaryLogicalState& result) const override;

   private:
    Pcf8574& extender_;
    const uint8_t port_;
  };

  /// Output-only port.
  ///
  /// Reading the state (getState()) returns the last set value without
  /// communicating with the extender. Prefer this to `Port` for output-only
  /// ports.
  class OutputPort : public BinarySwitch {
   public:
    OutputPort(Pcf8574& extender, uint8_t port);

    bool setState(BinaryLogicalState state) override;
    bool getState(BinaryLogicalState& result) const override;

   private:
    Pcf8574& extender_;
    const uint8_t port_;
  };

  /// Creates the extender on the specified TwoWire bus and I2C address.
  Pcf8574(TwoWire& wire, uint8_t addr);

  /// Returns the maximum allowed staleness of cached reads.
  ///
  /// Useful to reduce unnecessary repetitive I2C reads. Defaults to 20 ms.
  roo_time::Duration getReadCacheDuration() const {
    return last_read_cache_duration_;
  }

  /// Sets the maximum allowed staleness of cached reads.
  void setReadCacheDuration(roo_time::Duration duration) {
    last_read_cache_duration_ = duration;
  }

  /// Reads, caches, and returns levels of all ports.
  ///
  /// If called within the read-cache interval, may return cached results.
  /// Returns false on a communication failure.
  bool read(uint8_t& data);

  /// Writes the levels of all ports.
  ///
  /// For input ports, the corresponding bits should be HIGH to allow the slave
  /// to keep driving the actual line level. Returns false on failure.
  bool write(uint8_t data);

  /// Returns the most recently read byte, or 0xFF if never read.
  uint8_t last_read() const { return last_read_; }

  /// Returns the most recently written byte, or 0xFF if never written.
  uint8_t last_written() const { return last_written_; }

  /// Reads the level of the specified port.
  ///
  /// For output ports, the value generally reflects what was last written, but
  /// it is always read from the extender. On failure, returns the last known
  /// state.
  BinaryLogicalState readPort(uint8_t port);

  /// Writes the level of the specified port.
  ///
  /// Returns false on a communication failure. For input ports, it is OK to
  /// write HIGH to allow the slave to keep driving the level seen by readPort.
  /// Writing LOW will force the LOW state.
  bool writePort(uint8_t port, BinaryLogicalState state);

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
  roo_time::Duration last_read_cache_duration_;
};

}  // namespace roo_control