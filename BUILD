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
        "//lib/roo_collections",
        "//lib/roo_logging",
        "//lib/roo_onewire",
        "//lib/roo_prefs",
        "//lib/roo_scheduler",
        "//lib/roo_transceivers",
        "//roo_testing:arduino",
        "//roo_testing/frameworks/arduino-esp32-2.0.4/libraries/Wire",
        "@nanopb//:nanopb"
    ],
)
