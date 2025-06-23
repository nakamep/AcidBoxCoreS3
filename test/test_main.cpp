#include <unity.h>
#include <Arduino.h>

// Include test modules
#include "test_filters.h"
#include "test_effects.h"
#include "test_synthvoice.h"
#include "test_sampler.h"
#include "test_midi.h"
#include "test_audio_processing.h"

void setUp(void) {
    // Set up before each test
}

void tearDown(void) {
    // Clean up after each test
}

void setup() {
    delay(2000); // Give time for serial monitor to open
    
    UNITY_BEGIN();
    
    // Run all test suites
    run_filter_tests();
    run_effects_tests();
    run_synthvoice_tests();
    run_sampler_tests();
    run_midi_tests();
    run_audio_processing_tests();
    
    UNITY_END();
}

void loop() {
    // Empty loop for Arduino
}