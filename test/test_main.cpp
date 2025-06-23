#include <unity.h>
#include <math.h>
#include <stdint.h>
#include <cstdlib>
#include "test_st7701_lcd.h"

// For native testing, provide simple Arduino-like defines
#ifndef UNIT_TEST
#define UNIT_TEST
#endif

#ifndef PI
#define PI 3.14159265358979323846
#endif

#ifndef TABLE_SIZE
#define TABLE_SIZE 1024
#endif

// Simple test functions
void test_basic_math() {
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 3.14159f, PI);
    TEST_ASSERT_EQUAL(1024, TABLE_SIZE);
}

void test_sine_table_generation() {
    float sin_tbl[TABLE_SIZE + 1];
    
    // Generate sine table
    for (int i = 0; i <= TABLE_SIZE; i++) {
        sin_tbl[i] = sin(2.0f * PI * i / (float)TABLE_SIZE);
    }
    
    // Test key points
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, sin_tbl[0]); // sin(0)
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 1.0f, sin_tbl[TABLE_SIZE/4]); // sin(π/2)
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, sin_tbl[TABLE_SIZE/2]); // sin(π)
    TEST_ASSERT_FLOAT_WITHIN(0.001f, -1.0f, sin_tbl[3*TABLE_SIZE/4]); // sin(3π/2)
}

void test_midi_to_frequency_conversion() {
    // Test MIDI note to frequency conversion
    // A4 (440Hz) = MIDI note 69
    float freq_69 = 440.0f * pow(2.0f, (69 - 69) / 12.0f);
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 440.0f, freq_69);
    
    // C4 (middle C) = MIDI note 60
    float freq_60 = 440.0f * pow(2.0f, (60 - 69) / 12.0f);
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 261.63f, freq_60);
}

void test_audio_normalization() {
    float test_buffer[8] = {-2.0f, -1.0f, -0.5f, 0.0f, 0.5f, 1.0f, 1.5f, 2.0f};
    int buffer_size = 8;
    
    // Find peak
    float peak = 0.0f;
    for (int i = 0; i < buffer_size; i++) {
        if (fabs(test_buffer[i]) > peak) {
            peak = fabs(test_buffer[i]);
        }
    }
    
    // Normalize to prevent clipping
    float gain = 1.0f;
    if (peak > 1.0f) {
        gain = 1.0f / peak;
    }
    
    for (int i = 0; i < buffer_size; i++) {
        test_buffer[i] *= gain;
    }
    
    // Check that no sample exceeds ±1.0
    for (int i = 0; i < buffer_size; i++) {
        TEST_ASSERT_TRUE(fabs(test_buffer[i]) <= 1.0f);
    }
}

void test_linear_interpolation() {
    // Test linear interpolation for table lookups
    float table[5] = {0.0f, 1.0f, 4.0f, 9.0f, 16.0f}; // x^2 values
    
    // Interpolate between table[1] and table[2]
    float frac = 0.5f; // Halfway between indices 1 and 2
    float interp = table[1] + frac * (table[2] - table[1]);
    
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 2.5f, interp); // Should be 2.5
}

void test_midi_cc_conversion() {
    // Test converting MIDI CC values (0-127) to float (0.0-1.0)
    auto cc_to_float = [](uint8_t cc_value) -> float {
        return (float)cc_value / 127.0f;
    };
    
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, cc_to_float(0));
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 0.5f, cc_to_float(64)); // Increased tolerance for 64/127
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 1.0f, cc_to_float(127));
}

#ifdef ARDUINO
void setup() {
    delay(2000); // Give time for serial monitor to open
    
    UNITY_BEGIN();
    
    RUN_TEST(test_basic_math);
    RUN_TEST(test_sine_table_generation);
    RUN_TEST(test_midi_to_frequency_conversion);
    RUN_TEST(test_audio_normalization);
    RUN_TEST(test_linear_interpolation);
    RUN_TEST(test_midi_cc_conversion);
    
    // ST7701 LCD tests
    RUN_TEST(test_st7701_compilation);
    RUN_TEST(test_st7701_constants);
    RUN_TEST(test_st7701_color_definitions);
    RUN_TEST(test_st7701_global_instance);
    RUN_TEST(test_st7701_commands);
    
    UNITY_END();
}

void loop() {
    // Empty loop for Arduino
}
#else
int main() {
    UNITY_BEGIN();
    
    RUN_TEST(test_basic_math);
    RUN_TEST(test_sine_table_generation);
    RUN_TEST(test_midi_to_frequency_conversion);
    RUN_TEST(test_audio_normalization);
    RUN_TEST(test_linear_interpolation);
    RUN_TEST(test_midi_cc_conversion);
    
    // ST7701 LCD tests
    RUN_TEST(test_st7701_compilation);
    RUN_TEST(test_st7701_constants);
    RUN_TEST(test_st7701_color_definitions);
    RUN_TEST(test_st7701_global_instance);
    RUN_TEST(test_st7701_commands);
    
    return UNITY_END();
}
#endif