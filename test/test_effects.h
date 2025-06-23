#ifndef TEST_EFFECTS_H
#define TEST_EFFECTS_H

#include <unity.h>
#include <Arduino.h>

// Include effects headers
extern "C" {
    #include "../wavefolder.h"
    #include "../overdrive.h"
    // Note: delay and reverb may require PSRAM, so we'll mock or skip those
}

void test_wavefolder_init() {
    Wavefolder wf;
    wf.Init();
    TEST_ASSERT_TRUE(true); // Basic initialization
}

void test_wavefolder_no_folding() {
    Wavefolder wf;
    wf.Init();
    wf.SetDrive(0.0f);
    
    // Small input should pass through relatively unchanged
    float input = 0.5f;
    float output = wf.Process(input);
    TEST_ASSERT_FLOAT_WITHIN(0.1f, input, output);
}

void test_wavefolder_folding() {
    Wavefolder wf;
    wf.Init();
    wf.SetDrive(2.0f); // High drive to cause folding
    
    // Large input should be folded
    float input = 1.5f;
    float output = wf.Process(input);
    TEST_ASSERT_TRUE(abs(output) < abs(input)); // Folding should reduce amplitude
}

void test_wavefolder_symmetry() {
    Wavefolder wf;
    wf.Init();
    wf.SetDrive(1.5f);
    
    float pos_input = 1.2f;
    float neg_input = -1.2f;
    
    float pos_output = wf.Process(pos_input);
    float neg_output = wf.Process(neg_input);
    
    TEST_ASSERT_FLOAT_WITHIN(0.01f, abs(pos_output), abs(neg_output)); // Should be symmetric
}

void test_overdrive_init() {
    Overdrive od;
    od.Init();
    TEST_ASSERT_TRUE(true); // Basic initialization
}

void test_overdrive_clean() {
    Overdrive od;
    od.Init();
    od.SetDrive(0.0f); // Clean setting
    
    float input = 0.5f;
    float output = od.Process(input);
    TEST_ASSERT_FLOAT_WITHIN(0.2f, input, output); // Should be relatively clean
}

void test_overdrive_distortion() {
    Overdrive od;
    od.Init();
    od.SetDrive(0.8f); // High drive
    
    float input = 0.8f;
    float output = od.Process(input);
    
    // Overdrive should compress/saturate the signal
    TEST_ASSERT_TRUE(abs(output) <= abs(input) + 0.1f);
}

void test_overdrive_gain_staging() {
    Overdrive od;
    od.Init();
    
    // Test different drive levels
    float input = 0.5f;
    
    od.SetDrive(0.0f);
    float low_drive = od.Process(input);
    
    od.SetDrive(0.5f);
    float med_drive = od.Process(input);
    
    od.SetDrive(1.0f);
    float high_drive = od.Process(input);
    
    // Higher drive should generally produce more distortion/compression
    TEST_ASSERT_TRUE(abs(high_drive) >= abs(low_drive) - 0.1f);
}

void test_effects_chain_stability() {
    Wavefolder wf;
    Overdrive od;
    
    wf.Init();
    od.Init();
    
    wf.SetDrive(1.0f);
    od.SetDrive(0.7f);
    
    // Process a burst of samples through the chain
    float max_output = 0.0f;
    for (int i = 0; i < 1000; i++) {
        float input = sin(2.0f * PI * 220.0f * i / 44100.0f); // 220Hz sine
        float output = wf.Process(input);
        output = od.Process(output);
        
        if (abs(output) > max_output) {
            max_output = abs(output);
        }
    }
    
    TEST_ASSERT_TRUE(max_output < 5.0f); // Should not blow up
}

void test_effects_dc_blocking() {
    Overdrive od;
    od.Init();
    od.SetDrive(0.5f);
    
    // Process DC for a while
    float dc_sum = 0.0f;
    int samples = 100;
    
    for (int i = 0; i < samples; i++) {
        float output = od.Process(1.0f); // DC input
        dc_sum += output;
    }
    
    float dc_average = dc_sum / samples;
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 0.0f, dc_average); // Should not accumulate DC
}

void run_effects_tests() {
    RUN_TEST(test_wavefolder_init);
    RUN_TEST(test_wavefolder_no_folding);
    RUN_TEST(test_wavefolder_folding);
    RUN_TEST(test_wavefolder_symmetry);
    RUN_TEST(test_overdrive_init);
    RUN_TEST(test_overdrive_clean);
    RUN_TEST(test_overdrive_distortion);
    RUN_TEST(test_overdrive_gain_staging);
    RUN_TEST(test_effects_chain_stability);
    RUN_TEST(test_effects_dc_blocking);
}

#endif