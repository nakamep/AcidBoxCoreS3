#ifndef TEST_AUDIO_PROCESSING_H
#define TEST_AUDIO_PROCESSING_H

#include <unity.h>
#include <Arduino.h>
#include <math.h>

// Test lookup table generation and audio utility functions
void test_sine_table_generation() {
    const int TABLE_SIZE = 1024;
    float sin_tbl[TABLE_SIZE + 1];
    
    // Generate sine table (similar to tables.ino)
    for (int i = 0; i <= TABLE_SIZE; i++) {
        sin_tbl[i] = sin(2.0f * PI * i / (float)TABLE_SIZE);
    }
    
    // Test key points
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, sin_tbl[0]); // sin(0)
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 1.0f, sin_tbl[TABLE_SIZE/4]); // sin(π/2)
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, sin_tbl[TABLE_SIZE/2]); // sin(π)
    TEST_ASSERT_FLOAT_WITHIN(0.001f, -1.0f, sin_tbl[3*TABLE_SIZE/4]); // sin(3π/2)
}

void test_exp_table_generation() {
    const int TABLE_SIZE = 1024;
    float exp_tbl[TABLE_SIZE + 1];
    
    // Generate exponential table
    for (int i = 0; i <= TABLE_SIZE; i++) {
        float x = (float)i / (float)TABLE_SIZE;
        exp_tbl[i] = (exp(x) - 1.0f) / (exp(1.0f) - 1.0f); // Normalized 0-1
    }
    
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, exp_tbl[0]); // exp(0) normalized
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 1.0f, exp_tbl[TABLE_SIZE]); // exp(1) normalized
    TEST_ASSERT_TRUE(exp_tbl[TABLE_SIZE/2] > exp_tbl[TABLE_SIZE/4]); // Monotonic increase
}

void test_midi_to_frequency_conversion() {
    // Test MIDI note to frequency conversion
    // A4 (440Hz) = MIDI note 69
    float freq_69 = 440.0f * pow(2.0f, (69 - 69) / 12.0f);
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 440.0f, freq_69);
    
    // C4 (middle C) = MIDI note 60
    float freq_60 = 440.0f * pow(2.0f, (60 - 69) / 12.0f);
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 261.63f, freq_60);
    
    // Test octave doubling
    float freq_72 = 440.0f * pow(2.0f, (72 - 69) / 12.0f);
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 523.25f, freq_72); // C5
}

void test_audio_normalization() {
    float test_buffer[8] = {-2.0f, -1.0f, -0.5f, 0.0f, 0.5f, 1.0f, 1.5f, 2.0f};
    int buffer_size = 8;
    
    // Find peak
    float peak = 0.0f;
    for (int i = 0; i < buffer_size; i++) {
        if (abs(test_buffer[i]) > peak) {
            peak = abs(test_buffer[i]);
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
        TEST_ASSERT_TRUE(abs(test_buffer[i]) <= 1.0f);
    }
}

void test_linear_interpolation() {
    // Test linear interpolation for table lookups
    float table[5] = {0.0f, 1.0f, 4.0f, 9.0f, 16.0f}; // x^2 values
    
    // Interpolate between table[1] and table[2]
    float frac = 0.5f; // Halfway between indices 1 and 2
    float interp = table[1] + frac * (table[2] - table[1]);
    
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 2.5f, interp); // Should be 2.5
    
    // Test edge cases
    frac = 0.0f;
    interp = table[1] + frac * (table[2] - table[1]);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, table[1], interp);
    
    frac = 1.0f;
    interp = table[1] + frac * (table[2] - table[1]);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, table[2], interp);
}

void test_fast_shape_function() {
    // Test a basic shaping function (tanh approximation)
    auto fast_tanh = [](float x) -> float {
        if (x > 3.0f) return 1.0f;
        if (x < -3.0f) return -1.0f;
        float x2 = x * x;
        return x * (27.0f + x2) / (27.0f + 9.0f * x2); // Pade approximation
    };
    
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 0.0f, fast_tanh(0.0f));
    TEST_ASSERT_FLOAT_WITHIN(0.1f, tanh(1.0f), fast_tanh(1.0f));
    TEST_ASSERT_FLOAT_WITHIN(0.1f, tanh(-1.0f), fast_tanh(-1.0f));
    TEST_ASSERT_TRUE(abs(fast_tanh(5.0f)) <= 1.0f); // Should saturate
}

void test_audio_mixing() {
    float left_channel[4] = {0.5f, -0.3f, 0.8f, -0.6f};
    float right_channel[4] = {-0.2f, 0.7f, -0.4f, 0.9f};
    float mixed[4];
    
    // Simple stereo to mono mixing
    for (int i = 0; i < 4; i++) {
        mixed[i] = (left_channel[i] + right_channel[i]) * 0.5f;
    }
    
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.15f, mixed[0]); // (0.5 + -0.2) * 0.5
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.2f, mixed[1]);  // (-0.3 + 0.7) * 0.5
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.2f, mixed[2]);  // (0.8 + -0.4) * 0.5
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.15f, mixed[3]); // (-0.6 + 0.9) * 0.5
}

void test_dc_removal() {
    // Test DC removal filter (simple high-pass)
    float prev_input = 0.0f;
    float prev_output = 0.0f;
    float alpha = 0.995f; // DC removal coefficient
    
    float dc_input = 1.0f; // Pure DC
    float outputs[10];
    
    for (int i = 0; i < 10; i++) {
        float output = alpha * (prev_output + dc_input - prev_input);
        outputs[i] = output;
        prev_input = dc_input;
        prev_output = output;
    }
    
    // Output should decay towards zero for DC input
    TEST_ASSERT_TRUE(abs(outputs[9]) < abs(outputs[0]));
    TEST_ASSERT_TRUE(abs(outputs[9]) < 0.1f); // Should be small after several samples
}

void test_sample_rate_conversion() {
    // Test basic sample rate concepts
    float freq = 1000.0f;
    float sample_rate_44k = 44100.0f;
    float sample_rate_22k = 22050.0f;
    
    // Phase increment calculation
    float phase_inc_44k = 2.0f * PI * freq / sample_rate_44k;
    float phase_inc_22k = 2.0f * PI * freq / sample_rate_22k;
    
    // At half sample rate, phase increment should double
    TEST_ASSERT_FLOAT_WITHIN(0.001f, phase_inc_44k * 2.0f, phase_inc_22k);
    
    // Nyquist frequency checks
    float nyquist_44k = sample_rate_44k / 2.0f;
    float nyquist_22k = sample_rate_22k / 2.0f;
    
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 22050.0f, nyquist_44k);
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 11025.0f, nyquist_22k);
}

void run_audio_processing_tests() {
    RUN_TEST(test_sine_table_generation);
    RUN_TEST(test_exp_table_generation);
    RUN_TEST(test_midi_to_frequency_conversion);
    RUN_TEST(test_audio_normalization);
    RUN_TEST(test_linear_interpolation);
    RUN_TEST(test_fast_shape_function);
    RUN_TEST(test_audio_mixing);
    RUN_TEST(test_dc_removal);
    RUN_TEST(test_sample_rate_conversion);
}

#endif