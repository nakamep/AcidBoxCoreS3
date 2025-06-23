# AcidBox Unit Tests

This directory contains unit tests for the AcidBox ESP32 synthesizer project using the Unity testing framework.

## Test Structure

The tests are organized into several modules:

- **test_filters.h** - Tests for audio filters (Moog ladder, biquad, TeeBee filter)
- **test_effects.h** - Tests for audio effects (wavefolder, overdrive, distortion)
- **test_audio_processing.h** - Tests for core audio processing functions (lookup tables, conversions)
- **test_synthvoice.h** - Tests for TB-303 synthesizer voice functionality
- **test_sampler.h** - Tests for TR-808 drum sampler functionality
- **test_midi.h** - Tests for MIDI message handling and parameter mapping

## Running Tests

### Prerequisites

1. Install PlatformIO Core or use PlatformIO IDE
2. Ensure you have the Unity testing framework dependency

### Test Environments

Three test environments are configured:

#### ESP32 Hardware Tests
```bash
pio test -e test_esp32
```
Runs tests on actual ESP32 hardware. Requires connected ESP32 board.

#### ESP32-S3 Hardware Tests
```bash
pio test -e test_esp32s3
```
Runs tests on ESP32-S3 hardware. Requires connected ESP32-S3 board.

#### Native/Desktop Tests
```bash
pio test -e test_native
```
Runs tests on your local machine for faster development iteration.

### Running Specific Tests

To run a specific test file:
```bash
pio test -e test_native --filter test_filters
```

To run all tests with verbose output:
```bash
pio test -e test_native -v
```

## Test Categories

### Audio Processing Tests
- Lookup table generation (sine, exponential)
- MIDI to frequency conversion
- Audio normalization and mixing
- Sample rate conversion
- Linear interpolation

### Filter Tests
- Filter initialization and stability
- Frequency response verification
- Resonance behavior
- Cutoff frequency range testing

### Effects Tests
- Wavefolder distortion behavior
- Overdrive gain staging
- Effects chain stability
- DC blocking verification

### Synthesizer Tests
- Note on/off functionality
- Parameter control (volume, cutoff, resonance)
- Audio output generation
- Velocity response
- Slide/portamento behavior

### Sampler Tests
- Sample loading and playback
- Multi-sample triggering
- Stereo panning
- Pitch and decay control
- Effects send levels

### MIDI Tests
- Note on/off message handling
- Control change processing
- Channel filtering
- Parameter mapping verification
- Value range validation

## Mock Objects

Since this is an embedded project with hardware dependencies, many tests use mock objects to simulate:

- Audio filters and effects
- Sample data and playback
- MIDI message processing
- Hardware interfaces

## Test Configuration

### Build Flags
- `UNIT_TEST` - Enables test-specific code paths
- `TABLE_SIZE=1024` - Sets lookup table size for tests
- `PI=3.14159265358979323846` - Defines PI constant

### Dependencies
- Unity testing framework v2.5.2
- Standard C++ library for native tests

## Adding New Tests

1. Create a new test header file in the format `test_[module].h`
2. Include the header in `test_main.cpp`
3. Add a `run_[module]_tests()` function call to `setup()`
4. Follow the existing test patterns and naming conventions

### Test Function Naming
- Use descriptive names: `test_filter_cutoff_frequency()`
- Group related tests: `test_midi_note_on()`, `test_midi_note_off()`
- Include setup/teardown if needed

### Assertions
Use Unity assertion macros:
- `TEST_ASSERT_TRUE(condition)`
- `TEST_ASSERT_FALSE(condition)`
- `TEST_ASSERT_EQUAL(expected, actual)`
- `TEST_ASSERT_FLOAT_WITHIN(delta, expected, actual)`

## Continuous Integration

These tests can be integrated into CI/CD pipelines:

```yaml
# Example GitHub Actions workflow
- name: Run Tests
  run: |
    pio test -e test_native
    pio test -e test_esp32 --upload-port /dev/ttyUSB0
```

## Troubleshooting

### Common Issues

1. **Compilation Errors**: Ensure all dependencies are properly mocked for the test environment
2. **Timing Tests**: Use appropriate tolerances for floating-point comparisons
3. **Hardware Tests**: Verify proper board connection and port selection

### Debug Output

Enable verbose test output:
```bash
pio test -e test_native -v --verbose
```

For hardware debugging, use serial monitor:
```bash
pio device monitor --baud 115200
```

## Performance Considerations

- Native tests run fastest for development
- Hardware tests verify actual behavior on target platform
- Use appropriate timeouts for real-time audio processing tests
- Mock expensive operations (file I/O, PSRAM access) when possible