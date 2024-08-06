cc_library(
    name = "roo_control",
    srcs = glob(
        [
            "src/**/*.cpp",
            "src/**/*.c",
            "src/**/*.h",
            "src/**/*.inl",
        ],
        exclude = ["test/**"],
    ),
    includes = [
        "src",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//lib/roo_logging",
        "//lib/roo_scheduler",
        "//roo_testing:arduino",
        "//roo_testing/frameworks/arduino-esp32-2.0.4/libraries/Wire",
    ],
)
