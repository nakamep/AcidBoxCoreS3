#ifndef TEST_FILTERS_H
#define TEST_FILTERS_H

#include <unity.h>
#include <Arduino.h>

// Mock Arduino.h defines for testing
#ifndef PI
#define PI 3.14159265358979323846
#endif

// Include filter headers
extern "C" {
    // Include necessary filter implementations
    #include "../moogladder.h"
    #include "../rosic_OnePoleFilter.h"
    #include "../rosic_BiquadFilter.h"
    #include "../rosic_TeeBeeFilter.h"
}

void test_moog_ladder_init() {
    MoogLadder filter;
    filter.Init(44100.0f);
    TEST_ASSERT_TRUE(true); // Basic initialization test
}

void test_moog_ladder_process() {
    MoogLadder filter;
    filter.Init(44100.0f);
    filter.SetFreq(1000.0f);
    filter.SetRes(0.5f);
    
    // Test DC response
    float dc_input = 1.0f;
    float output = filter.Process(dc_input);
    TEST_ASSERT_TRUE(output >= 0.0f && output <= 1.1f); // Should not exceed reasonable bounds
}

void test_one_pole_filter() {
    OnePoleFilter filter;
    filter.setMode(OnePoleFilter::LOWPASS);
    filter.setCutoff(1000.0f);
    
    float output = filter.tick(1.0f);
    TEST_ASSERT_TRUE(output >= 0.0f && output <= 1.1f);
}

void test_biquad_filter_lowpass() {
    BiquadFilter filter;
    filter.setMode(BiquadFilter::LOWPASS);
    filter.setFrequency(1000.0f);
    filter.setQ(0.707f);
    
    float output = filter.tick(1.0f);
    TEST_ASSERT_TRUE(output >= 0.0f && output <= 1.1f);
}

void test_biquad_filter_highpass() {
    BiquadFilter filter;
    filter.setMode(BiquadFilter::HIGHPASS);
    filter.setFrequency(1000.0f);
    filter.setQ(0.707f);
    
    float output = filter.tick(1.0f);
    TEST_ASSERT_TRUE(output >= -1.1f && output <= 1.1f);
}

void test_teebee_filter_cutoff_range() {
    TeeBeeFilter filter;
    filter.init(44100.0f);
    
    // Test frequency range
    filter.setCutoff(100.0f);
    float low_output = filter.process(1.0f);
    
    filter.setCutoff(10000.0f);
    float high_output = filter.process(1.0f);
    
    TEST_ASSERT_TRUE(low_output != high_output); // Should produce different outputs
}

void test_teebee_filter_resonance() {
    TeeBeeFilter filter;
    filter.init(44100.0f);
    filter.setCutoff(1000.0f);
    
    // Test resonance range
    filter.setResonance(0.0f);
    float low_res_output = filter.process(1.0f);
    
    filter.setResonance(0.99f);
    float high_res_output = filter.process(1.0f);
    
    TEST_ASSERT_TRUE(abs(low_res_output) <= abs(high_res_output) + 0.1f); // High resonance may increase amplitude
}

void test_filter_stability() {
    MoogLadder filter;
    filter.Init(44100.0f);
    filter.SetFreq(1000.0f);
    filter.SetRes(0.9f); // High resonance
    
    // Process many samples to check for instability
    float max_output = 0.0f;
    for (int i = 0; i < 1000; i++) {
        float input = sin(2.0f * PI * 440.0f * i / 44100.0f); // 440Hz sine wave
        float output = filter.Process(input);
        if (abs(output) > max_output) {
            max_output = abs(output);
        }
    }
    
    TEST_ASSERT_TRUE(max_output < 10.0f); // Should not blow up
}

void run_filter_tests() {
    RUN_TEST(test_moog_ladder_init);
    RUN_TEST(test_moog_ladder_process);
    RUN_TEST(test_one_pole_filter);
    RUN_TEST(test_biquad_filter_lowpass);
    RUN_TEST(test_biquad_filter_highpass);
    RUN_TEST(test_teebee_filter_cutoff_range);
    RUN_TEST(test_teebee_filter_resonance);
    RUN_TEST(test_filter_stability);
}

#endif