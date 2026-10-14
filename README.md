# roo_control

Arduino relays, port extenders, debouncing, retries: common abstractions and
implementations.

## Documentation

* [Programming guide](doc/programming_guide.md)


## Bazel targets

Use `@roo_control` for the core control abstractions and drivers. Use
`@roo_control//:bound_transceivers` for `BoundSelector`, `BoundSwitch`, and
`BoundThermometer`; this target adds the `roo_transceivers` integration and
includes the core target. Existing header include paths are unchanged.

## Host emulation

Host builds use the roo_testing 2.0 Arduino ESP32 profile. With Bazelisk 1.21
or newer, a plain command defaults to that profile and prints a notice:

    bazel test ...
    bazel test ... --config=asan
    bazel test ... --config=roo_testing_arduino_esp32

The files under .roo_testing/bazelrc/esp32 are vendored from roo_testing;
follow their canonical-source headers when refreshing them.
