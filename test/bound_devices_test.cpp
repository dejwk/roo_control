#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "roo_control/switch/bound_switch.h"
#include "roo_control/thermometer/bound_thermometer.h"

namespace roo_control {
namespace {
using namespace roo_transceivers;
using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;

class MockStore : public BindingStore {
 public:
  MOCK_METHOD(SensorLocator, getSensorBinding, (SensorKey), (override));
  MOCK_METHOD(void, setSensorBinding, (SensorKey, const SensorLocator&),
              (override));
  MOCK_METHOD(void, clearSensorBinding, (SensorKey), (override));
  MOCK_METHOD(ActuatorLocator, getActuatorBinding, (ActuatorKey), (override));
  MOCK_METHOD(void, setActuatorBinding, (ActuatorKey, const ActuatorLocator&),
              (override));
  MOCK_METHOD(void, clearActuatorBinding, (ActuatorKey), (override));
  MOCK_METHOD(DeviceLocator, getDeviceBinding, (DeviceKey), (override));
  MOCK_METHOD(void, setDeviceBinding, (DeviceKey, const DeviceLocator&),
              (override));
  MOCK_METHOD(void, clearDeviceBinding, (DeviceKey), (override));
};

class MockUniverse : public Universe {
 public:
  MOCK_METHOD(size_t, deviceCount, (), (const, override));
  MOCK_METHOD(bool, forEachDevice, (std::function<bool(const DeviceLocator&)>),
              (const, override));
  MOCK_METHOD(bool, getDeviceDescriptor, (const DeviceLocator&, Descriptor&),
              (const, override));
  MOCK_METHOD(Measurement, read, (const SensorLocator&), (const, override));
  MOCK_METHOD(bool, write, (const ActuatorLocator&, float), (override));
  MOCK_METHOD(void, requestUpdate, (), (override));
};

class BoundDevicesTest : public ::testing::Test {
 protected:
  BoundDevicesTest() : actuator_(store_, 1), sensor_(store_, 2) {
    actuator_.bind(ActuatorLocator("relay", "pool", "1"));
    sensor_.bind(SensorLocator("thermometer", "pool", "1"));
  }

  void setReading(Quantity quantity, float value) {
    ON_CALL(universe_, read(_))
        .WillByDefault(Return(Measurement(
            quantity, roo_time::Uptime::Start() + roo_time::Seconds(5),
            value)));
  }

  NiceMock<MockStore> store_;
  NiceMock<MockUniverse> universe_;
  ActuatorBinding actuator_;
  SensorBinding sensor_;
};

// Verifies binary switches accept both current protocol state quantities.
TEST_F(BoundDevicesTest, BinaryAndMultiStateReadings) {
  BoundBinarySwitch sw(universe_, &actuator_);
  BinaryLogicalState state;
  setReading(Quantity::kBinaryState, 1);
  ASSERT_TRUE(sw.getState(state));
  EXPECT_EQ(BINARY_STATE_HIGH, state);
  setReading(Quantity::kMultiState, 0);
  ASSERT_TRUE(sw.getState(state));
  EXPECT_EQ(BINARY_STATE_LOW, state);
}

// Verifies mismatched quantities and non-binary values fail without changing
// output.
TEST_F(BoundDevicesTest, RejectsInvalidBinaryReadings) {
  BoundBinarySwitch sw(universe_, &actuator_);
  BinaryLogicalState state = BINARY_STATE_HIGH;
  setReading(Quantity::kTemperature, 0);
  EXPECT_FALSE(sw.getState(state));
  EXPECT_EQ(BINARY_STATE_HIGH, state);
  setReading(Quantity::kBinaryState, 2);
  EXPECT_FALSE(sw.getState(state));
  EXPECT_EQ(BINARY_STATE_HIGH, state);
}

// Verifies generic switches preserve integer multi-state readings and reject
// fractions.
TEST_F(BoundDevicesTest, IntegerMultiStateReadings) {
  BoundSwitch<int> sw(universe_, &actuator_);
  int state = 0;
  setReading(Quantity::kMultiState, 3);
  ASSERT_TRUE(sw.getState(state));
  EXPECT_EQ(3, state);
  setReading(Quantity::kMultiState, 1.5f);
  EXPECT_FALSE(sw.getState(state));
  EXPECT_EQ(3, state);
}

// Verifies a current protocol temperature retains its value and measurement
// time.
TEST_F(BoundDevicesTest, TemperatureReading) {
  BoundThermometer thermometer(universe_, &sensor_);
  setReading(Quantity::kTemperature, 27.5f);
  const Thermometer::Reading reading = thermometer.readTemperature();
  EXPECT_FLOAT_EQ(27.5f, reading.value.degCelcius());
  EXPECT_EQ(roo_time::Uptime::Start() + roo_time::Seconds(5), reading.time);
}

}  // namespace
}  // namespace roo_control
