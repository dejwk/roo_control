# Programming guide

`roo_control` is a utility library that gives you a few small interfaces and
wrappers for embedded control code, so the rest of your program does not need
to hard-code knowledge of GPIO pins, port expanders, or remote transports.

Most of the library is centered on discrete I/O:

* `Selector<State>` represents a readable discrete input, such as a button,
  contact sensor, or small enumerated mode selector.
* `Switch<State>` represents a discrete output or settable state, such as a
  relay, output port, or small multi-state actuator.

`Thermometer` is another abstraction that models a timestamped temperature
measurement.

Around these utility abstractions, `roo_control` provides a few practical
building blocks:

* GPIO-backed implementations for common binary inputs and outputs.
* `PushButton`, which turns a raw binary selector into debounced button events.
* `InertSwitch`, which enforces a minimum delay between state changes and can
  retry failed writes.
* `Pcf8574`, which exposes an I2C port expander through the same switch and
  selector interfaces.
* adapters for `roo_transceivers`, so the same application logic can work with
  remote devices.

This guide introduces the library incrementally. We will start with raw GPIO,
then add button semantics, then wrap outputs for safer hardware control, and
only then move on to port expanders and remote bindings.

## Part 1: Getting started

### The mental model

Before wiring anything, it helps to understand the layering.

At the bottom, you have concrete hardware-facing classes such as `GpioRelay`,
`GpioConnector`, and `Pcf8574::Port`.

In the middle, you have wrappers such as `PushButton`, `InertSwitch`, and
`ExpiringThermometer`. These do not care whether the underlying device is local
GPIO, an expander, or something remote. They only depend on the abstract
interfaces.

At the top, you have your application logic. In most control code, that logic
should talk in terms of `BinarySelector` and `BinarySwitch`, rather than
specific pins or buses. Temperature handling usually sits alongside that logic,
but it is a different shape of problem because it deals with timestamped
measurements rather than discrete states.

That separation is the main reason to use `roo_control`.

### Your first switch

The simplest output class in the library is `GpioRelay`. It implements a
binary `Switch`, so it can both set and report its current state.

```cpp
#include <Arduino.h>

#include "roo_control/switch/gpio_relay.h"

using namespace roo_control;

static constexpr uint8_t kRelayPin = 5;

GpioRelay relay(kRelayPin, BINARY_STATE_LOW);

void setup() {}

void loop() {
  relay.setState(BINARY_STATE_HIGH);
  delay(1000);

  relay.setState(BINARY_STATE_LOW);
  delay(1000);
}
```

This does three things for you:

* it configures the pin as output in the constructor,
* it gives the pin a meaningful interface,
* it makes later refactoring easier because the rest of the code can depend on
  `BinarySwitch` instead of direct `digitalWrite()` calls.

Binary state is represented by `BinaryLogicalState`, which has two values:
`BINARY_STATE_LOW` and `BINARY_STATE_HIGH`. These map naturally to the digital
logic levels you already use with Arduino.

`setState()` returns `true` on success. For `GpioRelay`, success means that the
pin was written and read back as expected.

### Your first selector

The input counterpart of `GpioRelay` is `GpioConnector`. It is a raw
`BinarySelector`: it simply reports the current digital level on a pin.

```cpp
#include <Arduino.h>

#include "roo_control/selector/gpio_connector.h"

using namespace roo_control;

static constexpr uint8_t kButtonPin = 4;

GpioConnector button(kButtonPin, INPUT_PULLUP);

void setup() {
  Serial.begin(115200);
}

void loop() {
  BinaryLogicalState state;
  if (button.getState(state)) {
    Serial.println(state == BINARY_STATE_LOW ? "pressed" : "released");
  }
  delay(50);
}
```

This example uses `INPUT_PULLUP`, which is a common way to wire a button:

* unpressed button => pin reads `HIGH`,
* pressed button => pin reads `LOW`.

At this level, there is no debouncing, no click detection, and no interpretation
of the signal. `GpioConnector` only exposes the raw state.

That is exactly what you want from a low-level selector.

### From raw levels to button events

Real buttons are noisy. They bounce, and most applications care about events
such as click or long-press rather than raw electrical transitions.

`PushButton` wraps a `BinarySelector` and adds:

* debouncing,
* `onDown()` and `onUp()` hooks,
* `onClick()` and `onDoubleClick()` hooks,
* `onLongPress()` hook.

To use it, derive a class and override the callbacks you need.

```cpp
#include <Arduino.h>

#include "roo_control/selector/gpio_connector.h"
#include "roo_control/selector/push_button.h"

using namespace roo_control;

static constexpr uint8_t kButtonPin = 4;

GpioConnector raw_button(kButtonPin, INPUT_PULLUP);

class LoggingButton : public PushButton {
 public:
  LoggingButton(BinarySelector& selector)
      : PushButton(selector, BINARY_STATE_HIGH) {}

 protected:
  void onDown() override { Serial.println("down"); }
  void onUp() override { Serial.println("up"); }
  void onClick() override { Serial.println("click"); }
  void onLongPress() override { Serial.println("long press"); }
  void onDoubleClick() override { Serial.println("double click"); }
};

LoggingButton button(raw_button);

void setup() {
  Serial.begin(115200);
}

void loop() {
  button.tick();
  delay(5);
}
```

The important rule is that `tick()` must run periodically, ideally every 5 to
10 ms. That is how `PushButton` performs debouncing and gesture detection.

The second constructor argument is the idle state. With pull-up wiring, the
button rests at `HIGH`, so `BINARY_STATE_HIGH` is correct. If your hardware is
wired the other way around, use `BINARY_STATE_LOW`.

`PushButton` is intentionally callback-based. It fits microcontroller programs
well because there is usually a small number of physical buttons and each one
has device-specific meaning.

### First complete example: button toggles a relay

Now we can connect input and output through the abstractions.

```cpp
#include <Arduino.h>

#include "roo_control/selector/gpio_connector.h"
#include "roo_control/selector/push_button.h"
#include "roo_control/switch/gpio_relay.h"

using namespace roo_control;

static constexpr uint8_t kButtonPin = 4;
static constexpr uint8_t kRelayPin = 5;

GpioConnector raw_button(kButtonPin, INPUT_PULLUP);
GpioRelay relay(kRelayPin, BINARY_STATE_LOW);

class ToggleButton : public PushButton {
 public:
  ToggleButton(BinarySelector& selector, BinarySwitch& relay)
      : PushButton(selector, BINARY_STATE_HIGH), relay_(relay), on_(false) {}

 protected:
  void onClick() override {
    on_ = !on_;
    relay_.setState(on_ ? BINARY_STATE_HIGH : BINARY_STATE_LOW);
  }

 private:
  BinarySwitch& relay_;
  bool on_;
};

ToggleButton button(raw_button, relay);

void setup() {}

void loop() {
  button.tick();
  delay(5);
}
```

This is already a useful pattern. The button logic depends only on
`BinarySelector` and `BinarySwitch`, so later you can replace either the input
or the output without rewriting the interaction logic.

### Protecting real relays with `InertSwitch`

GPIO outputs are simple, but many real actuators should not be toggled too
quickly. Mechanical relays are the classic example. Repeated fast transitions
can stress the hardware, and when the underlying device is remote or flaky you
may also want retries.

`InertSwitch` wraps any `Switch<State>` and adds two behaviors:

* it enforces a minimum delay between actual state changes,
* if the underlying `setState()` reports failure, it schedules retries using a
  backoff policy.

Because retries and delayed state changes need time management, `InertSwitch`
depends on `roo_scheduler`.

```cpp
#include <Arduino.h>

#include "roo_control/selector/gpio_connector.h"
#include "roo_control/selector/push_button.h"
#include "roo_control/switch/gpio_relay.h"
#include "roo_control/switch/inert_switch.h"
#include "roo_scheduler.h"
#include "roo_time.h"

using namespace roo_control;
using namespace roo_scheduler;
using namespace roo_time;

static constexpr uint8_t kButtonPin = 4;
static constexpr uint8_t kRelayPin = 5;

Scheduler scheduler;
GpioConnector raw_button(kButtonPin, INPUT_PULLUP);
GpioRelay raw_relay(kRelayPin, BINARY_STATE_LOW);
InertBinarySwitch relay(scheduler, raw_relay, Millis(500));

class ToggleButton : public PushButton {
 public:
  ToggleButton(BinarySelector& selector, BinarySwitch& relay)
      : PushButton(selector, BINARY_STATE_HIGH), relay_(relay), on_(false) {}

 protected:
  void onClick() override {
    on_ = !on_;
    relay_.setState(on_ ? BINARY_STATE_HIGH : BINARY_STATE_LOW);
  }

 private:
  BinarySwitch& relay_;
  bool on_;
};

ToggleButton button(raw_button, relay);

void setup() {}

void loop() {
  button.tick();
  scheduler.delay(Millis(5));
}
```

Two details matter here:

* `relay.setState()` updates the intended state immediately, but the physical
  relay may switch later if inertia delays it.
* `scheduler.delay()` is not just a delay; it also executes scheduled work. If
  you stop running the scheduler, deferred state changes and retries will never
  happen.

For a GPIO-backed relay, failures are uncommon. The real value of
`InertSwitch` appears when the wrapped actuator is mechanical, remote, or both.

## Part 2: Common building blocks

### Program to interfaces, not pins

The most useful habit with `roo_control` is to write application logic against
the abstract interfaces.

```cpp
#include "roo_control/selector/selector.h"
#include "roo_control/switch/switch.h"

using namespace roo_control;

bool isActive(const BinarySelector& selector) {
  BinaryLogicalState state;
  return selector.getState(state) && state == BINARY_STATE_HIGH;
}

bool setEnabled(BinarySwitch& sw, bool enabled) {
  return sw.setState(enabled ? BINARY_STATE_HIGH : BINARY_STATE_LOW);
}
```

Once your logic looks like this, you can swap the implementation underneath it:

* `GpioRelay` today,
* `Pcf8574::OutputPort` tomorrow,
* `BoundBinarySwitch` later, if the device moves into another MCU.

That is the core design payoff of the library.

### Scaling beyond direct GPIO: `Pcf8574`

When you run out of GPIO pins, a port expander is often the next step.
`roo_control` includes support for the PCF8574 I2C expander.

```cpp
#include <Arduino.h>
#include <Wire.h>

#include "roo_control/port_extender/pcf8574.h"

using namespace roo_control;

Pcf8574 extender(Wire, 0x20);
Pcf8574::OutputPort relay(extender, 0);
Pcf8574::Port contact(extender, 1);

void setup() {
  Wire.begin();
  relay.setState(BINARY_STATE_LOW);
}

void loop() {
  BinaryLogicalState state;
  if (contact.getState(state)) {
    relay.setState(state);
  }
  delay(10);
}
```

There are two port wrapper types:

* `Pcf8574::Port`: for quasi-bidirectional ports that may be used as input or
  output.
* `Pcf8574::OutputPort`: for output-only usage when you only care about the
  last written value.

The distinction matters because the PCF8574 is not a normal direction-based
GPIO block. For an input line, you generally leave the corresponding bit high
and let the external circuit pull it low when needed. `Port` reflects that
model more accurately.

`Pcf8574` also caches reads for a short interval by default. That reduces I2C
traffic when multiple callers poll the same device frequently.

### Multi-state selectors and switches

Most examples in this guide are binary because that is the most common embedded
control case. The interfaces themselves are generic, though:

* `Selector<State>` reads a value of type `State`.
* `Switch<State>` reads and writes a value of type `State`.

That makes them suitable for things like fan speeds, operating modes, or other
small enumerated states.

```cpp
enum FanMode { kFanOff = 0, kFanLow = 1, kFanHigh = 2 };

class MemoryFanMode : public roo_control::Switch<FanMode> {
 public:
  bool getState(FanMode& result) const override {
    result = state_;
    return true;
  }

  bool setState(FanMode state) override {
    state_ = state;
    return true;
  }

 private:
  FanMode state_ = kFanOff;
};
```

Binary aliases such as `BinarySelector` and `BinarySwitch` are just convenient
specializations of those generic interfaces.

### Thermometers and stale readings

Temperature is modeled slightly differently from a selector because a useful
temperature reading needs both a value and the time it was measured.

The `Thermometer` interface returns a `Reading`:

* `value`: a `roo_quantity::Temperature`,
* `time`: the timestamp of the reading.

`roo_control` includes `ExpiringThermometer`, which wraps another thermometer
and suppresses stale readings.

```cpp
#include <Arduino.h>

#include "roo_control/thermometer/expiring_thermometer.h"

using namespace roo_control;

class DemoThermometer : public Thermometer {
 public:
  Reading readTemperature() const override {
    return Reading{.value = roo_quantity::TemperatureDegCelcius(21.5f),
                   .time = roo_time::Uptime::Now()};
  }
};

DemoThermometer raw_thermometer;
ExpiringThermometer safe_thermometer(&raw_thermometer, roo_time::Seconds(30));

void setup() {
  Serial.begin(115200);
}

void loop() {
  Thermometer::Reading reading = safe_thermometer.readTemperature();
  if (reading.value.isUnknown()) {
    Serial.println("temperature unavailable");
  } else {
    Serial.println(String(reading.value.degCelcius()));
  }
  delay(1000);
}
```

`ExpiringThermometer` is useful when the underlying data source may temporarily
disappear. It keeps using the most recent known reading while it is still fresh,
and turns it into `UnknownTemperature()` once it is too old.

That gives your application a clean rule: recent data is usable, stale data is
not.

### Failure handling

`roo_control` uses simple, explicit failure signals.

For selectors:

* `getState()` returns `false` when the current state cannot be read.

For switches:

* `setState()` returns `false` when the write fails.

For thermometers:

* the thermometer always returns a `Reading`, but the temperature may be
  unknown.

Treat these as normal runtime conditions, not as exceptional situations. On
real hardware, communication glitches, stale remote state, or temporarily
unavailable sensors are part of the environment.

## Part 3: Integration and extension

### Adapting `roo_transceivers`

`roo_control` includes adapters that bridge the library to
`roo_transceivers`:

* `BoundSwitch` for remotely bound actuators,
* `BoundThermometer` for remotely bound temperature sensors,
* `BoundSelector` for remotely bound selectors.

The important distinction here is that `roo_transceivers` is broader than
`roo_control`. It models generic sensor measurements and actuator values,
including continuous quantities. The adapters in `roo_control` are convenience
views that map that broader model into the narrower utility interfaces used in
this guide: discrete selectors, discrete switches, and timestamped temperature
readings.

The practical payoff is that once a remote device is adapted to
`BinarySwitch`, `BinarySelector`, or `Thermometer`, the rest of your
application can stay unchanged.

For example, code written against `BinarySwitch& pump` does not need to know
whether `pump` is implemented with:

* a local GPIO pin,
* a PCF8574 output,
* a remote actuator discovered through a transceiver universe.

That is often the point where `roo_control` starts paying for itself.

### Writing your own implementations

The interfaces are intentionally small.

To add your own selector, implement:

```cpp
template <typename State>
class Selector {
 public:
  virtual bool getState(State& result) const = 0;
};
```

To add your own switch, implement:

```cpp
template <typename State>
class Switch : public Selector<State> {
 public:
  virtual bool setState(State state) = 0;
};
```

To add your own thermometer, implement:

```cpp
class Thermometer {
 public:
  struct Reading {
    roo_quantity::Temperature value;
    roo_time::Uptime time;
  };

  virtual Reading readTemperature() const = 0;
};
```

The behavioral contracts are straightforward:

* return `false` from selectors or switches when the operation could not be
  completed,
* return an unknown temperature when the thermometer cannot produce a usable
  value,
* use timestamps that describe when the measurement was actually obtained, not
  just when it was queried.

Once those contracts are respected, the existing wrappers keep working.

### A practical layering pattern

In real projects, a clean structure often looks like this:

* hardware layer: `GpioConnector`, `GpioRelay`, `Pcf8574::Port`, or custom
  device drivers,
* behavior layer: `PushButton`, `InertSwitch`, `ExpiringThermometer`,
* application layer: business logic written against `BinarySelector`,
  `BinarySwitch`, or `Thermometer`.

That structure keeps the top of the program stable even when the hardware below
it changes.

### Troubleshooting

#### `PushButton` never fires callbacks

Check these first:

* `tick()` must run every 5 to 10 ms.
* the idle state must match the wiring.
* with pull-up wiring, a pressed button usually reads `LOW`.

#### `InertSwitch` never applies deferred changes

Make sure the scheduler is actually running. Calling `setState()` on an
`InertSwitch` is not enough by itself; deferred work runs only while
`roo_scheduler::Scheduler` is being serviced.

#### A PCF8574 input looks wrong

Remember that PCF8574 pins are quasi-bidirectional. If you want to use a line
as input, it generally must be left high so the external circuit can pull it
low.

#### Temperature stays unavailable

Check both parts of the reading:

* does the underlying thermometer produce a non-unknown value?
* does it attach a reasonable timestamp?

If either part is missing, `ExpiringThermometer` cannot help.

## Conclusion

`roo_control` does not try to be a huge hardware abstraction layer. Its scope
is much narrower and more useful: provide a few small utility abstractions,
mostly around discrete inputs and outputs, plus a timestamped temperature
reading interface and a handful of composable wrappers that let embedded
application logic stay clean while the hardware underneath evolves.

If you take one idea from this guide, make it this one: keep your application
logic dependent on `BinarySelector`, `BinarySwitch`, and `Thermometer`, and let
the concrete GPIO, I2C, or remote-device details live at the edges.