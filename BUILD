load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("@rules_cc//cc:cc_test.bzl", "cc_test")

_BOUND_DEVICE_HEADERS = [
    "src/roo_control/selector/bound_selector.h",
    "src/roo_control/switch/bound_switch.h",
    "src/roo_control/thermometer/bound_thermometer.h",
]

cc_library(
    name = "roo_control",
    srcs = glob(
        [
            "src/**/*.cpp",
            "src/**/*.h",
        ],
        exclude = ["test/**"] + _BOUND_DEVICE_HEADERS,
    ),
    includes = [
        "src",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@roo_logging",
        "@roo_quantity",
        "@roo_scheduler",
        "@roo_testing//:arduino",
        "@roo_testing//roo_testing/frameworks/arduino-esp32-2.0.4/libraries/Wire",
    ],
)

cc_library(
    name = "bound_transceivers",
    hdrs = _BOUND_DEVICE_HEADERS,
    visibility = ["//visibility:public"],
    deps = [
        ":roo_control",
        "@roo_transceivers",
    ],
)

cc_test(
    name = "bound_devices_test",
    srcs = ["test/bound_devices_test.cpp"],
    linkstatic = 1,
    deps = [
        ":bound_transceivers",
        "@googletest//:gtest_main",
    ],
)
