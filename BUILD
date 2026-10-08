load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("@rules_cc//cc:cc_test.bzl", "cc_test")

cc_library(
    name = "roo_control",
    srcs = glob(
        [
            "src/**/*.cpp",
            "src/**/*.h",
        ],
        exclude = ["test/**"],
    ),
    includes = [
        "src",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@roo_logging",
        "@roo_quantity",
        "@roo_scheduler",
        "@roo_transceivers",
        "@roo_testing//:arduino",
        "@roo_testing//roo_testing/frameworks/arduino-esp32-2.0.4/libraries/Wire",
    ],
)

cc_test(
    name = "bound_devices_test",
    srcs = ["test/bound_devices_test.cpp"],
    linkstatic = 1,
    deps = [
        ":roo_control",
        "@googletest//:gtest_main",
    ],
)
