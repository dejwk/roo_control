/* Automatically generated nanopb header */
/* Generated by nanopb-0.4.9.1 */

#ifndef PB_ROO_CONTROL_TRANSCEIVERS_PB_H_INCLUDED
#define PB_ROO_CONTROL_TRANSCEIVERS_PB_H_INCLUDED
#include "pb.h"

#if PB_PROTO_HEADER_VERSION != 40
#error Regenerate this file with the current version of nanopb generator.
#endif

/* Enum definitions */
typedef enum _roo_control_Quantity {
    roo_control_Quantity_kUnspecifiedQuantity = 0,
    /* Fundamental. */
    roo_control_Quantity_kElapsedTime = 1, /* [seconds]. */
    roo_control_Quantity_kMass = 2, /* [kilograms]. */
    roo_control_Quantity_kTemperature = 3, /* [deg C]. */
    roo_control_Quantity_kFrequency = 4, /* [Hz]. */
    roo_control_Quantity_kLength = 5, /* [m]. */
    roo_control_Quantity_kAngle = 6, /* [rad]. */
    /* Displacement. */
    roo_control_Quantity_kDistance = 10, /* [m]. */
    roo_control_Quantity_kVelocity = 11, /* [m/s]. */
    roo_control_Quantity_kAcceleration = 12, /* [m/s^2]. */
    roo_control_Quantity_kJerk = 13, /* [m/s^3]. */
    /* Force and energy. */
    roo_control_Quantity_kForce = 20, /* [N]. */
    roo_control_Quantity_kPressure = 21, /* [Pa]. */
    roo_control_Quantity_kEnergy = 22, /* [J]. */
    roo_control_Quantity_kPower = 23, /* [W]. */
    /* Spatial. */
    roo_control_Quantity_kArea = 30, /* [m^2]. */
    roo_control_Quantity_kVolume = 31, /* [m^3]. */
    /* Liquids. */
    roo_control_Quantity_kLiquidVolume = 40, /* [liter]. */
    roo_control_Quantity_kLiquidFlow = 41, /* [liter/min]. */
    /* Thermodynamics. */
    roo_control_Quantity_kHeat = 50, /* [J]. */
    /* Magnetism. */
    roo_control_Quantity_kMagneticFlux = 60, /* [We]. */
    /* Electricity. */
    roo_control_Quantity_kElectricCharge = 70, /* [C]. */
    roo_control_Quantity_kElectricCurrent = 71, /* [A]. */
    roo_control_Quantity_kVoltage = 72, /* [V]. */
    roo_control_Quantity_kResistance = 73, /* [Ohm]. */
    roo_control_Quantity_kImpedance = 74, /* [Ohm]. */
    roo_control_Quantity_kCapacitance = 75, /* [F]. */
    roo_control_Quantity_kInductance = 76, /* [T]. */
    /* Light. */
    roo_control_Quantity_kLuminousIntensity = 90, /* [lumen]. */
    roo_control_Quantity_kColorHue = 95, /* [0-360). */
    roo_control_Quantity_kColorSaturation = 96, /* [0-1]. */
    roo_control_Quantity_kColorBrightness = 97, /* [0-1]. */
    /* Geolocation. */
    roo_control_Quantity_kLatitude = 110, /* [-90-90] (North is positive). */
    roo_control_Quantity_kLongitude = 111, /* [-180-180] (East is positive). */
    roo_control_Quantity_kAltitude = 112, /* [m]. */
    /* Environmental. */
    roo_control_Quantity_kAirHumidity = 130, /* [%]. */
    /* Generic control. */
    roo_control_Quantity_kBinaryState = 150, /* on/off (0.0 or 1.0). */
    roo_control_Quantity_kMultiState = 151
} roo_control_Quantity;

/* Struct definitions */
typedef struct _roo_control_TransceiverDescriptor_Sensor {
    char id[24];
    roo_control_Quantity quantity;
} roo_control_TransceiverDescriptor_Sensor;

typedef struct _roo_control_TransceiverDescriptor_Actuator {
    char id[24];
    roo_control_Quantity quantity;
} roo_control_TransceiverDescriptor_Actuator;

typedef struct _roo_control_TransceiverDescriptor {
    pb_size_t sensors_count;
    roo_control_TransceiverDescriptor_Sensor sensors[16];
    pb_size_t actuators_count;
    roo_control_TransceiverDescriptor_Actuator actuators[16];
} roo_control_TransceiverDescriptor;

typedef struct _roo_control_TransceiverClientMessage_RequestUpdate {
    char dummy_field;
} roo_control_TransceiverClientMessage_RequestUpdate;

typedef struct _roo_control_TransceiverClientMessage_RequestState {
    char dummy_field;
} roo_control_TransceiverClientMessage_RequestState;

typedef struct _roo_control_TransceiverClientMessage_Write {
    char device_locator_schema[16];
    char device_locator_id[24];
    char device_locator_actuator_id[24];
    float value;
} roo_control_TransceiverClientMessage_Write;

typedef struct _roo_control_TransceiverClientMessage {
    pb_size_t which_contents;
    union {
        roo_control_TransceiverClientMessage_RequestUpdate request_update;
        roo_control_TransceiverClientMessage_RequestState request_state;
        roo_control_TransceiverClientMessage_Write write;
    } contents;
} roo_control_TransceiverClientMessage;

typedef struct _roo_control_TransceiverServerMessage_TransceiverUpdateBegin {
    bool delta;
} roo_control_TransceiverServerMessage_TransceiverUpdateBegin;

typedef struct _roo_control_TransceiverServerMessage_DescriptorAdded {
    uint32_t key;
    bool has_descriptor;
    roo_control_TransceiverDescriptor descriptor;
} roo_control_TransceiverServerMessage_DescriptorAdded;

typedef struct _roo_control_TransceiverServerMessage_DescriptorRemoved {
    uint32_t key;
} roo_control_TransceiverServerMessage_DescriptorRemoved;

typedef struct _roo_control_TransceiverServerMessage_DeviceAdded {
    char locator_schema[16];
    char locator_id[24];
    uint32_t descriptor_key;
} roo_control_TransceiverServerMessage_DeviceAdded;

typedef struct _roo_control_TransceiverServerMessage_DeviceRemoved {
    uint32_t prev_index;
} roo_control_TransceiverServerMessage_DeviceRemoved;

typedef struct _roo_control_TransceiverServerMessage_DevicePreserved {
    uint32_t prev_index;
    bool has_count;
    uint32_t count;
} roo_control_TransceiverServerMessage_DevicePreserved;

typedef struct _roo_control_TransceiverServerMessage_DeviceModified {
    uint32_t prev_index;
    uint32_t descriptor_key;
} roo_control_TransceiverServerMessage_DeviceModified;

typedef struct _roo_control_TransceiverServerMessage_TransceiverUpdateEnd {
    char dummy_field;
} roo_control_TransceiverServerMessage_TransceiverUpdateEnd;

typedef struct _roo_control_TransceiverServerMessage_ReadingsBegin {
    char dummy_field;
} roo_control_TransceiverServerMessage_ReadingsBegin;

typedef struct _roo_control_TransceiverServerMessage_ReadingsEnd {
    char dummy_field;
} roo_control_TransceiverServerMessage_ReadingsEnd;

typedef struct _roo_control_TransceiverServerMessage_Reading_SensorValue {
    char device_locator_sensor_id[24];
    float value;
    uint64_t age_ms;
} roo_control_TransceiverServerMessage_Reading_SensorValue;

typedef struct _roo_control_TransceiverServerMessage_Reading {
    char device_locator_schema[16];
    char device_locator_id[24];
    pb_size_t sensor_values_count;
    roo_control_TransceiverServerMessage_Reading_SensorValue sensor_values[16];
} roo_control_TransceiverServerMessage_Reading;

typedef struct _roo_control_TransceiverServerMessage {
    pb_size_t which_contents;
    union {
        roo_control_TransceiverServerMessage_TransceiverUpdateBegin transceiver_update_begin;
        roo_control_TransceiverServerMessage_DescriptorAdded descriptor_added;
        roo_control_TransceiverServerMessage_DescriptorRemoved descriptor_removed;
        /* Device device = 4; */
        roo_control_TransceiverServerMessage_DeviceAdded device_added;
        roo_control_TransceiverServerMessage_DeviceRemoved device_removed;
        roo_control_TransceiverServerMessage_DevicePreserved device_preserved;
        roo_control_TransceiverServerMessage_DeviceModified device_modified;
        roo_control_TransceiverServerMessage_TransceiverUpdateEnd transceiver_update_end;
        roo_control_TransceiverServerMessage_ReadingsBegin readings_begin;
        roo_control_TransceiverServerMessage_Reading reading;
        roo_control_TransceiverServerMessage_ReadingsEnd readings_end;
    } contents;
} roo_control_TransceiverServerMessage;


#ifdef __cplusplus
extern "C" {
#endif

/* Helper constants for enums */
#define _roo_control_Quantity_MIN roo_control_Quantity_kUnspecifiedQuantity
#define _roo_control_Quantity_MAX roo_control_Quantity_kMultiState
#define _roo_control_Quantity_ARRAYSIZE ((roo_control_Quantity)(roo_control_Quantity_kMultiState+1))


#define roo_control_TransceiverDescriptor_Sensor_quantity_ENUMTYPE roo_control_Quantity

#define roo_control_TransceiverDescriptor_Actuator_quantity_ENUMTYPE roo_control_Quantity



















/* Initializer values for message structs */
#define roo_control_TransceiverDescriptor_init_default {0, {roo_control_TransceiverDescriptor_Sensor_init_default, roo_control_TransceiverDescriptor_Sensor_init_default, roo_control_TransceiverDescriptor_Sensor_init_default, roo_control_TransceiverDescriptor_Sensor_init_default, roo_control_TransceiverDescriptor_Sensor_init_default, roo_control_TransceiverDescriptor_Sensor_init_default, roo_control_TransceiverDescriptor_Sensor_init_default, roo_control_TransceiverDescriptor_Sensor_init_default, roo_control_TransceiverDescriptor_Sensor_init_default, roo_control_TransceiverDescriptor_Sensor_init_default, roo_control_TransceiverDescriptor_Sensor_init_default, roo_control_TransceiverDescriptor_Sensor_init_default, roo_control_TransceiverDescriptor_Sensor_init_default, roo_control_TransceiverDescriptor_Sensor_init_default, roo_control_TransceiverDescriptor_Sensor_init_default, roo_control_TransceiverDescriptor_Sensor_init_default}, 0, {roo_control_TransceiverDescriptor_Actuator_init_default, roo_control_TransceiverDescriptor_Actuator_init_default, roo_control_TransceiverDescriptor_Actuator_init_default, roo_control_TransceiverDescriptor_Actuator_init_default, roo_control_TransceiverDescriptor_Actuator_init_default, roo_control_TransceiverDescriptor_Actuator_init_default, roo_control_TransceiverDescriptor_Actuator_init_default, roo_control_TransceiverDescriptor_Actuator_init_default, roo_control_TransceiverDescriptor_Actuator_init_default, roo_control_TransceiverDescriptor_Actuator_init_default, roo_control_TransceiverDescriptor_Actuator_init_default, roo_control_TransceiverDescriptor_Actuator_init_default, roo_control_TransceiverDescriptor_Actuator_init_default, roo_control_TransceiverDescriptor_Actuator_init_default, roo_control_TransceiverDescriptor_Actuator_init_default, roo_control_TransceiverDescriptor_Actuator_init_default}}
#define roo_control_TransceiverDescriptor_Sensor_init_default {"", _roo_control_Quantity_MIN}
#define roo_control_TransceiverDescriptor_Actuator_init_default {"", _roo_control_Quantity_MIN}
#define roo_control_TransceiverClientMessage_init_default {0, {roo_control_TransceiverClientMessage_RequestUpdate_init_default}}
#define roo_control_TransceiverClientMessage_RequestUpdate_init_default {0}
#define roo_control_TransceiverClientMessage_RequestState_init_default {0}
#define roo_control_TransceiverClientMessage_Write_init_default {"", "", "", 0}
#define roo_control_TransceiverServerMessage_init_default {0, {roo_control_TransceiverServerMessage_TransceiverUpdateBegin_init_default}}
#define roo_control_TransceiverServerMessage_TransceiverUpdateBegin_init_default {0}
#define roo_control_TransceiverServerMessage_DescriptorAdded_init_default {0, false, roo_control_TransceiverDescriptor_init_default}
#define roo_control_TransceiverServerMessage_DescriptorRemoved_init_default {0}
#define roo_control_TransceiverServerMessage_DeviceAdded_init_default {"", "", 0}
#define roo_control_TransceiverServerMessage_DeviceRemoved_init_default {0}
#define roo_control_TransceiverServerMessage_DevicePreserved_init_default {0, false, 0}
#define roo_control_TransceiverServerMessage_DeviceModified_init_default {0, 0}
#define roo_control_TransceiverServerMessage_TransceiverUpdateEnd_init_default {0}
#define roo_control_TransceiverServerMessage_ReadingsBegin_init_default {0}
#define roo_control_TransceiverServerMessage_ReadingsEnd_init_default {0}
#define roo_control_TransceiverServerMessage_Reading_init_default {"", "", 0, {roo_control_TransceiverServerMessage_Reading_SensorValue_init_default, roo_control_TransceiverServerMessage_Reading_SensorValue_init_default, roo_control_TransceiverServerMessage_Reading_SensorValue_init_default, roo_control_TransceiverServerMessage_Reading_SensorValue_init_default, roo_control_TransceiverServerMessage_Reading_SensorValue_init_default, roo_control_TransceiverServerMessage_Reading_SensorValue_init_default, roo_control_TransceiverServerMessage_Reading_SensorValue_init_default, roo_control_TransceiverServerMessage_Reading_SensorValue_init_default, roo_control_TransceiverServerMessage_Reading_SensorValue_init_default, roo_control_TransceiverServerMessage_Reading_SensorValue_init_default, roo_control_TransceiverServerMessage_Reading_SensorValue_init_default, roo_control_TransceiverServerMessage_Reading_SensorValue_init_default, roo_control_TransceiverServerMessage_Reading_SensorValue_init_default, roo_control_TransceiverServerMessage_Reading_SensorValue_init_default, roo_control_TransceiverServerMessage_Reading_SensorValue_init_default, roo_control_TransceiverServerMessage_Reading_SensorValue_init_default}}
#define roo_control_TransceiverServerMessage_Reading_SensorValue_init_default {"", 0, 0}
#define roo_control_TransceiverDescriptor_init_zero {0, {roo_control_TransceiverDescriptor_Sensor_init_zero, roo_control_TransceiverDescriptor_Sensor_init_zero, roo_control_TransceiverDescriptor_Sensor_init_zero, roo_control_TransceiverDescriptor_Sensor_init_zero, roo_control_TransceiverDescriptor_Sensor_init_zero, roo_control_TransceiverDescriptor_Sensor_init_zero, roo_control_TransceiverDescriptor_Sensor_init_zero, roo_control_TransceiverDescriptor_Sensor_init_zero, roo_control_TransceiverDescriptor_Sensor_init_zero, roo_control_TransceiverDescriptor_Sensor_init_zero, roo_control_TransceiverDescriptor_Sensor_init_zero, roo_control_TransceiverDescriptor_Sensor_init_zero, roo_control_TransceiverDescriptor_Sensor_init_zero, roo_control_TransceiverDescriptor_Sensor_init_zero, roo_control_TransceiverDescriptor_Sensor_init_zero, roo_control_TransceiverDescriptor_Sensor_init_zero}, 0, {roo_control_TransceiverDescriptor_Actuator_init_zero, roo_control_TransceiverDescriptor_Actuator_init_zero, roo_control_TransceiverDescriptor_Actuator_init_zero, roo_control_TransceiverDescriptor_Actuator_init_zero, roo_control_TransceiverDescriptor_Actuator_init_zero, roo_control_TransceiverDescriptor_Actuator_init_zero, roo_control_TransceiverDescriptor_Actuator_init_zero, roo_control_TransceiverDescriptor_Actuator_init_zero, roo_control_TransceiverDescriptor_Actuator_init_zero, roo_control_TransceiverDescriptor_Actuator_init_zero, roo_control_TransceiverDescriptor_Actuator_init_zero, roo_control_TransceiverDescriptor_Actuator_init_zero, roo_control_TransceiverDescriptor_Actuator_init_zero, roo_control_TransceiverDescriptor_Actuator_init_zero, roo_control_TransceiverDescriptor_Actuator_init_zero, roo_control_TransceiverDescriptor_Actuator_init_zero}}
#define roo_control_TransceiverDescriptor_Sensor_init_zero {"", _roo_control_Quantity_MIN}
#define roo_control_TransceiverDescriptor_Actuator_init_zero {"", _roo_control_Quantity_MIN}
#define roo_control_TransceiverClientMessage_init_zero {0, {roo_control_TransceiverClientMessage_RequestUpdate_init_zero}}
#define roo_control_TransceiverClientMessage_RequestUpdate_init_zero {0}
#define roo_control_TransceiverClientMessage_RequestState_init_zero {0}
#define roo_control_TransceiverClientMessage_Write_init_zero {"", "", "", 0}
#define roo_control_TransceiverServerMessage_init_zero {0, {roo_control_TransceiverServerMessage_TransceiverUpdateBegin_init_zero}}
#define roo_control_TransceiverServerMessage_TransceiverUpdateBegin_init_zero {0}
#define roo_control_TransceiverServerMessage_DescriptorAdded_init_zero {0, false, roo_control_TransceiverDescriptor_init_zero}
#define roo_control_TransceiverServerMessage_DescriptorRemoved_init_zero {0}
#define roo_control_TransceiverServerMessage_DeviceAdded_init_zero {"", "", 0}
#define roo_control_TransceiverServerMessage_DeviceRemoved_init_zero {0}
#define roo_control_TransceiverServerMessage_DevicePreserved_init_zero {0, false, 0}
#define roo_control_TransceiverServerMessage_DeviceModified_init_zero {0, 0}
#define roo_control_TransceiverServerMessage_TransceiverUpdateEnd_init_zero {0}
#define roo_control_TransceiverServerMessage_ReadingsBegin_init_zero {0}
#define roo_control_TransceiverServerMessage_ReadingsEnd_init_zero {0}
#define roo_control_TransceiverServerMessage_Reading_init_zero {"", "", 0, {roo_control_TransceiverServerMessage_Reading_SensorValue_init_zero, roo_control_TransceiverServerMessage_Reading_SensorValue_init_zero, roo_control_TransceiverServerMessage_Reading_SensorValue_init_zero, roo_control_TransceiverServerMessage_Reading_SensorValue_init_zero, roo_control_TransceiverServerMessage_Reading_SensorValue_init_zero, roo_control_TransceiverServerMessage_Reading_SensorValue_init_zero, roo_control_TransceiverServerMessage_Reading_SensorValue_init_zero, roo_control_TransceiverServerMessage_Reading_SensorValue_init_zero, roo_control_TransceiverServerMessage_Reading_SensorValue_init_zero, roo_control_TransceiverServerMessage_Reading_SensorValue_init_zero, roo_control_TransceiverServerMessage_Reading_SensorValue_init_zero, roo_control_TransceiverServerMessage_Reading_SensorValue_init_zero, roo_control_TransceiverServerMessage_Reading_SensorValue_init_zero, roo_control_TransceiverServerMessage_Reading_SensorValue_init_zero, roo_control_TransceiverServerMessage_Reading_SensorValue_init_zero, roo_control_TransceiverServerMessage_Reading_SensorValue_init_zero}}
#define roo_control_TransceiverServerMessage_Reading_SensorValue_init_zero {"", 0, 0}

/* Field tags (for use in manual encoding/decoding) */
#define roo_control_TransceiverDescriptor_Sensor_id_tag 1
#define roo_control_TransceiverDescriptor_Sensor_quantity_tag 2
#define roo_control_TransceiverDescriptor_Actuator_id_tag 1
#define roo_control_TransceiverDescriptor_Actuator_quantity_tag 2
#define roo_control_TransceiverDescriptor_sensors_tag 3
#define roo_control_TransceiverDescriptor_actuators_tag 4
#define roo_control_TransceiverClientMessage_Write_device_locator_schema_tag 1
#define roo_control_TransceiverClientMessage_Write_device_locator_id_tag 2
#define roo_control_TransceiverClientMessage_Write_device_locator_actuator_id_tag 3
#define roo_control_TransceiverClientMessage_Write_value_tag 4
#define roo_control_TransceiverClientMessage_request_update_tag 1
#define roo_control_TransceiverClientMessage_request_state_tag 2
#define roo_control_TransceiverClientMessage_write_tag 3
#define roo_control_TransceiverServerMessage_TransceiverUpdateBegin_delta_tag 1
#define roo_control_TransceiverServerMessage_DescriptorAdded_key_tag 1
#define roo_control_TransceiverServerMessage_DescriptorAdded_descriptor_tag 2
#define roo_control_TransceiverServerMessage_DescriptorRemoved_key_tag 1
#define roo_control_TransceiverServerMessage_DeviceAdded_locator_schema_tag 1
#define roo_control_TransceiverServerMessage_DeviceAdded_locator_id_tag 2
#define roo_control_TransceiverServerMessage_DeviceAdded_descriptor_key_tag 3
#define roo_control_TransceiverServerMessage_DeviceRemoved_prev_index_tag 1
#define roo_control_TransceiverServerMessage_DevicePreserved_prev_index_tag 1
#define roo_control_TransceiverServerMessage_DevicePreserved_count_tag 2
#define roo_control_TransceiverServerMessage_DeviceModified_prev_index_tag 1
#define roo_control_TransceiverServerMessage_DeviceModified_descriptor_key_tag 3
#define roo_control_TransceiverServerMessage_Reading_SensorValue_device_locator_sensor_id_tag 1
#define roo_control_TransceiverServerMessage_Reading_SensorValue_value_tag 2
#define roo_control_TransceiverServerMessage_Reading_SensorValue_age_ms_tag 3
#define roo_control_TransceiverServerMessage_Reading_device_locator_schema_tag 1
#define roo_control_TransceiverServerMessage_Reading_device_locator_id_tag 2
#define roo_control_TransceiverServerMessage_Reading_sensor_values_tag 3
#define roo_control_TransceiverServerMessage_transceiver_update_begin_tag 1
#define roo_control_TransceiverServerMessage_descriptor_added_tag 2
#define roo_control_TransceiverServerMessage_descriptor_removed_tag 3
#define roo_control_TransceiverServerMessage_device_added_tag 5
#define roo_control_TransceiverServerMessage_device_removed_tag 6
#define roo_control_TransceiverServerMessage_device_preserved_tag 7
#define roo_control_TransceiverServerMessage_device_modified_tag 8
#define roo_control_TransceiverServerMessage_transceiver_update_end_tag 9
#define roo_control_TransceiverServerMessage_readings_begin_tag 10
#define roo_control_TransceiverServerMessage_reading_tag 11
#define roo_control_TransceiverServerMessage_readings_end_tag 12

/* Struct field encoding specification for nanopb */
#define roo_control_TransceiverDescriptor_FIELDLIST(X, a) \
X(a, STATIC,   REPEATED, MESSAGE,  sensors,           3) \
X(a, STATIC,   REPEATED, MESSAGE,  actuators,         4)
#define roo_control_TransceiverDescriptor_CALLBACK NULL
#define roo_control_TransceiverDescriptor_DEFAULT NULL
#define roo_control_TransceiverDescriptor_sensors_MSGTYPE roo_control_TransceiverDescriptor_Sensor
#define roo_control_TransceiverDescriptor_actuators_MSGTYPE roo_control_TransceiverDescriptor_Actuator

#define roo_control_TransceiverDescriptor_Sensor_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, STRING,   id,                1) \
X(a, STATIC,   SINGULAR, UENUM,    quantity,          2)
#define roo_control_TransceiverDescriptor_Sensor_CALLBACK NULL
#define roo_control_TransceiverDescriptor_Sensor_DEFAULT NULL

#define roo_control_TransceiverDescriptor_Actuator_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, STRING,   id,                1) \
X(a, STATIC,   SINGULAR, UENUM,    quantity,          2)
#define roo_control_TransceiverDescriptor_Actuator_CALLBACK NULL
#define roo_control_TransceiverDescriptor_Actuator_DEFAULT NULL

#define roo_control_TransceiverClientMessage_FIELDLIST(X, a) \
X(a, STATIC,   ONEOF,    MESSAGE,  (contents,request_update,contents.request_update),   1) \
X(a, STATIC,   ONEOF,    MESSAGE,  (contents,request_state,contents.request_state),   2) \
X(a, STATIC,   ONEOF,    MESSAGE,  (contents,write,contents.write),   3)
#define roo_control_TransceiverClientMessage_CALLBACK NULL
#define roo_control_TransceiverClientMessage_DEFAULT NULL
#define roo_control_TransceiverClientMessage_contents_request_update_MSGTYPE roo_control_TransceiverClientMessage_RequestUpdate
#define roo_control_TransceiverClientMessage_contents_request_state_MSGTYPE roo_control_TransceiverClientMessage_RequestState
#define roo_control_TransceiverClientMessage_contents_write_MSGTYPE roo_control_TransceiverClientMessage_Write

#define roo_control_TransceiverClientMessage_RequestUpdate_FIELDLIST(X, a) \

#define roo_control_TransceiverClientMessage_RequestUpdate_CALLBACK NULL
#define roo_control_TransceiverClientMessage_RequestUpdate_DEFAULT NULL

#define roo_control_TransceiverClientMessage_RequestState_FIELDLIST(X, a) \

#define roo_control_TransceiverClientMessage_RequestState_CALLBACK NULL
#define roo_control_TransceiverClientMessage_RequestState_DEFAULT NULL

#define roo_control_TransceiverClientMessage_Write_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, STRING,   device_locator_schema,   1) \
X(a, STATIC,   SINGULAR, STRING,   device_locator_id,   2) \
X(a, STATIC,   SINGULAR, STRING,   device_locator_actuator_id,   3) \
X(a, STATIC,   SINGULAR, FLOAT,    value,             4)
#define roo_control_TransceiverClientMessage_Write_CALLBACK NULL
#define roo_control_TransceiverClientMessage_Write_DEFAULT NULL

#define roo_control_TransceiverServerMessage_FIELDLIST(X, a) \
X(a, STATIC,   ONEOF,    MESSAGE,  (contents,transceiver_update_begin,contents.transceiver_update_begin),   1) \
X(a, STATIC,   ONEOF,    MESSAGE,  (contents,descriptor_added,contents.descriptor_added),   2) \
X(a, STATIC,   ONEOF,    MESSAGE,  (contents,descriptor_removed,contents.descriptor_removed),   3) \
X(a, STATIC,   ONEOF,    MESSAGE,  (contents,device_added,contents.device_added),   5) \
X(a, STATIC,   ONEOF,    MESSAGE,  (contents,device_removed,contents.device_removed),   6) \
X(a, STATIC,   ONEOF,    MESSAGE,  (contents,device_preserved,contents.device_preserved),   7) \
X(a, STATIC,   ONEOF,    MESSAGE,  (contents,device_modified,contents.device_modified),   8) \
X(a, STATIC,   ONEOF,    MESSAGE,  (contents,transceiver_update_end,contents.transceiver_update_end),   9) \
X(a, STATIC,   ONEOF,    MESSAGE,  (contents,readings_begin,contents.readings_begin),  10) \
X(a, STATIC,   ONEOF,    MESSAGE,  (contents,reading,contents.reading),  11) \
X(a, STATIC,   ONEOF,    MESSAGE,  (contents,readings_end,contents.readings_end),  12)
#define roo_control_TransceiverServerMessage_CALLBACK NULL
#define roo_control_TransceiverServerMessage_DEFAULT NULL
#define roo_control_TransceiverServerMessage_contents_transceiver_update_begin_MSGTYPE roo_control_TransceiverServerMessage_TransceiverUpdateBegin
#define roo_control_TransceiverServerMessage_contents_descriptor_added_MSGTYPE roo_control_TransceiverServerMessage_DescriptorAdded
#define roo_control_TransceiverServerMessage_contents_descriptor_removed_MSGTYPE roo_control_TransceiverServerMessage_DescriptorRemoved
#define roo_control_TransceiverServerMessage_contents_device_added_MSGTYPE roo_control_TransceiverServerMessage_DeviceAdded
#define roo_control_TransceiverServerMessage_contents_device_removed_MSGTYPE roo_control_TransceiverServerMessage_DeviceRemoved
#define roo_control_TransceiverServerMessage_contents_device_preserved_MSGTYPE roo_control_TransceiverServerMessage_DevicePreserved
#define roo_control_TransceiverServerMessage_contents_device_modified_MSGTYPE roo_control_TransceiverServerMessage_DeviceModified
#define roo_control_TransceiverServerMessage_contents_transceiver_update_end_MSGTYPE roo_control_TransceiverServerMessage_TransceiverUpdateEnd
#define roo_control_TransceiverServerMessage_contents_readings_begin_MSGTYPE roo_control_TransceiverServerMessage_ReadingsBegin
#define roo_control_TransceiverServerMessage_contents_reading_MSGTYPE roo_control_TransceiverServerMessage_Reading
#define roo_control_TransceiverServerMessage_contents_readings_end_MSGTYPE roo_control_TransceiverServerMessage_ReadingsEnd

#define roo_control_TransceiverServerMessage_TransceiverUpdateBegin_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, BOOL,     delta,             1)
#define roo_control_TransceiverServerMessage_TransceiverUpdateBegin_CALLBACK NULL
#define roo_control_TransceiverServerMessage_TransceiverUpdateBegin_DEFAULT NULL

#define roo_control_TransceiverServerMessage_DescriptorAdded_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, UINT32,   key,               1) \
X(a, STATIC,   OPTIONAL, MESSAGE,  descriptor,        2)
#define roo_control_TransceiverServerMessage_DescriptorAdded_CALLBACK NULL
#define roo_control_TransceiverServerMessage_DescriptorAdded_DEFAULT NULL
#define roo_control_TransceiverServerMessage_DescriptorAdded_descriptor_MSGTYPE roo_control_TransceiverDescriptor

#define roo_control_TransceiverServerMessage_DescriptorRemoved_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, UINT32,   key,               1)
#define roo_control_TransceiverServerMessage_DescriptorRemoved_CALLBACK NULL
#define roo_control_TransceiverServerMessage_DescriptorRemoved_DEFAULT NULL

#define roo_control_TransceiverServerMessage_DeviceAdded_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, STRING,   locator_schema,    1) \
X(a, STATIC,   SINGULAR, STRING,   locator_id,        2) \
X(a, STATIC,   SINGULAR, UINT32,   descriptor_key,    3)
#define roo_control_TransceiverServerMessage_DeviceAdded_CALLBACK NULL
#define roo_control_TransceiverServerMessage_DeviceAdded_DEFAULT NULL

#define roo_control_TransceiverServerMessage_DeviceRemoved_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, UINT32,   prev_index,        1)
#define roo_control_TransceiverServerMessage_DeviceRemoved_CALLBACK NULL
#define roo_control_TransceiverServerMessage_DeviceRemoved_DEFAULT NULL

#define roo_control_TransceiverServerMessage_DevicePreserved_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, UINT32,   prev_index,        1) \
X(a, STATIC,   OPTIONAL, UINT32,   count,             2)
#define roo_control_TransceiverServerMessage_DevicePreserved_CALLBACK NULL
#define roo_control_TransceiverServerMessage_DevicePreserved_DEFAULT NULL

#define roo_control_TransceiverServerMessage_DeviceModified_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, UINT32,   prev_index,        1) \
X(a, STATIC,   SINGULAR, UINT32,   descriptor_key,    3)
#define roo_control_TransceiverServerMessage_DeviceModified_CALLBACK NULL
#define roo_control_TransceiverServerMessage_DeviceModified_DEFAULT NULL

#define roo_control_TransceiverServerMessage_TransceiverUpdateEnd_FIELDLIST(X, a) \

#define roo_control_TransceiverServerMessage_TransceiverUpdateEnd_CALLBACK NULL
#define roo_control_TransceiverServerMessage_TransceiverUpdateEnd_DEFAULT NULL

#define roo_control_TransceiverServerMessage_ReadingsBegin_FIELDLIST(X, a) \

#define roo_control_TransceiverServerMessage_ReadingsBegin_CALLBACK NULL
#define roo_control_TransceiverServerMessage_ReadingsBegin_DEFAULT NULL

#define roo_control_TransceiverServerMessage_ReadingsEnd_FIELDLIST(X, a) \

#define roo_control_TransceiverServerMessage_ReadingsEnd_CALLBACK NULL
#define roo_control_TransceiverServerMessage_ReadingsEnd_DEFAULT NULL

#define roo_control_TransceiverServerMessage_Reading_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, STRING,   device_locator_schema,   1) \
X(a, STATIC,   SINGULAR, STRING,   device_locator_id,   2) \
X(a, STATIC,   REPEATED, MESSAGE,  sensor_values,     3)
#define roo_control_TransceiverServerMessage_Reading_CALLBACK NULL
#define roo_control_TransceiverServerMessage_Reading_DEFAULT NULL
#define roo_control_TransceiverServerMessage_Reading_sensor_values_MSGTYPE roo_control_TransceiverServerMessage_Reading_SensorValue

#define roo_control_TransceiverServerMessage_Reading_SensorValue_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, STRING,   device_locator_sensor_id,   1) \
X(a, STATIC,   SINGULAR, FLOAT,    value,             2) \
X(a, STATIC,   SINGULAR, UINT64,   age_ms,            3)
#define roo_control_TransceiverServerMessage_Reading_SensorValue_CALLBACK NULL
#define roo_control_TransceiverServerMessage_Reading_SensorValue_DEFAULT NULL

extern const pb_msgdesc_t roo_control_TransceiverDescriptor_msg;
extern const pb_msgdesc_t roo_control_TransceiverDescriptor_Sensor_msg;
extern const pb_msgdesc_t roo_control_TransceiverDescriptor_Actuator_msg;
extern const pb_msgdesc_t roo_control_TransceiverClientMessage_msg;
extern const pb_msgdesc_t roo_control_TransceiverClientMessage_RequestUpdate_msg;
extern const pb_msgdesc_t roo_control_TransceiverClientMessage_RequestState_msg;
extern const pb_msgdesc_t roo_control_TransceiverClientMessage_Write_msg;
extern const pb_msgdesc_t roo_control_TransceiverServerMessage_msg;
extern const pb_msgdesc_t roo_control_TransceiverServerMessage_TransceiverUpdateBegin_msg;
extern const pb_msgdesc_t roo_control_TransceiverServerMessage_DescriptorAdded_msg;
extern const pb_msgdesc_t roo_control_TransceiverServerMessage_DescriptorRemoved_msg;
extern const pb_msgdesc_t roo_control_TransceiverServerMessage_DeviceAdded_msg;
extern const pb_msgdesc_t roo_control_TransceiverServerMessage_DeviceRemoved_msg;
extern const pb_msgdesc_t roo_control_TransceiverServerMessage_DevicePreserved_msg;
extern const pb_msgdesc_t roo_control_TransceiverServerMessage_DeviceModified_msg;
extern const pb_msgdesc_t roo_control_TransceiverServerMessage_TransceiverUpdateEnd_msg;
extern const pb_msgdesc_t roo_control_TransceiverServerMessage_ReadingsBegin_msg;
extern const pb_msgdesc_t roo_control_TransceiverServerMessage_ReadingsEnd_msg;
extern const pb_msgdesc_t roo_control_TransceiverServerMessage_Reading_msg;
extern const pb_msgdesc_t roo_control_TransceiverServerMessage_Reading_SensorValue_msg;

/* Defines for backwards compatibility with code written before nanopb-0.4.0 */
#define roo_control_TransceiverDescriptor_fields &roo_control_TransceiverDescriptor_msg
#define roo_control_TransceiverDescriptor_Sensor_fields &roo_control_TransceiverDescriptor_Sensor_msg
#define roo_control_TransceiverDescriptor_Actuator_fields &roo_control_TransceiverDescriptor_Actuator_msg
#define roo_control_TransceiverClientMessage_fields &roo_control_TransceiverClientMessage_msg
#define roo_control_TransceiverClientMessage_RequestUpdate_fields &roo_control_TransceiverClientMessage_RequestUpdate_msg
#define roo_control_TransceiverClientMessage_RequestState_fields &roo_control_TransceiverClientMessage_RequestState_msg
#define roo_control_TransceiverClientMessage_Write_fields &roo_control_TransceiverClientMessage_Write_msg
#define roo_control_TransceiverServerMessage_fields &roo_control_TransceiverServerMessage_msg
#define roo_control_TransceiverServerMessage_TransceiverUpdateBegin_fields &roo_control_TransceiverServerMessage_TransceiverUpdateBegin_msg
#define roo_control_TransceiverServerMessage_DescriptorAdded_fields &roo_control_TransceiverServerMessage_DescriptorAdded_msg
#define roo_control_TransceiverServerMessage_DescriptorRemoved_fields &roo_control_TransceiverServerMessage_DescriptorRemoved_msg
#define roo_control_TransceiverServerMessage_DeviceAdded_fields &roo_control_TransceiverServerMessage_DeviceAdded_msg
#define roo_control_TransceiverServerMessage_DeviceRemoved_fields &roo_control_TransceiverServerMessage_DeviceRemoved_msg
#define roo_control_TransceiverServerMessage_DevicePreserved_fields &roo_control_TransceiverServerMessage_DevicePreserved_msg
#define roo_control_TransceiverServerMessage_DeviceModified_fields &roo_control_TransceiverServerMessage_DeviceModified_msg
#define roo_control_TransceiverServerMessage_TransceiverUpdateEnd_fields &roo_control_TransceiverServerMessage_TransceiverUpdateEnd_msg
#define roo_control_TransceiverServerMessage_ReadingsBegin_fields &roo_control_TransceiverServerMessage_ReadingsBegin_msg
#define roo_control_TransceiverServerMessage_ReadingsEnd_fields &roo_control_TransceiverServerMessage_ReadingsEnd_msg
#define roo_control_TransceiverServerMessage_Reading_fields &roo_control_TransceiverServerMessage_Reading_msg
#define roo_control_TransceiverServerMessage_Reading_SensorValue_fields &roo_control_TransceiverServerMessage_Reading_SensorValue_msg

/* Maximum encoded size of messages (where known) */
#define ROO_CONTROL_TRANSCEIVERS_PB_H_MAX_SIZE   roo_control_TransceiverServerMessage_size
#define roo_control_TransceiverClientMessage_RequestState_size 0
#define roo_control_TransceiverClientMessage_RequestUpdate_size 0
#define roo_control_TransceiverClientMessage_Write_size 72
#define roo_control_TransceiverClientMessage_size 74
#define roo_control_TransceiverDescriptor_Actuator_size 28
#define roo_control_TransceiverDescriptor_Sensor_size 28
#define roo_control_TransceiverDescriptor_size   960
#define roo_control_TransceiverServerMessage_DescriptorAdded_size 969
#define roo_control_TransceiverServerMessage_DescriptorRemoved_size 6
#define roo_control_TransceiverServerMessage_DeviceAdded_size 48
#define roo_control_TransceiverServerMessage_DeviceModified_size 12
#define roo_control_TransceiverServerMessage_DevicePreserved_size 12
#define roo_control_TransceiverServerMessage_DeviceRemoved_size 6
#define roo_control_TransceiverServerMessage_Reading_SensorValue_size 41
#define roo_control_TransceiverServerMessage_Reading_size 730
#define roo_control_TransceiverServerMessage_ReadingsBegin_size 0
#define roo_control_TransceiverServerMessage_ReadingsEnd_size 0
#define roo_control_TransceiverServerMessage_TransceiverUpdateBegin_size 2
#define roo_control_TransceiverServerMessage_TransceiverUpdateEnd_size 0
#define roo_control_TransceiverServerMessage_size 972

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
