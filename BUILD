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
        "@roo_testing//:arduino",
        "@roo_testing//roo_testing/frameworks/arduino-esp32-2.0.4/libraries/Wire",
    ],
)
