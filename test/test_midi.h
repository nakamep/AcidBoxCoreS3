#ifndef TEST_MIDI_H
#define TEST_MIDI_H

#include <unity.h>
#include <Arduino.h>

// Mock MIDI message structure
struct MockMidiMessage {
    uint8_t type;
    uint8_t channel;
    uint8_t data1;
    uint8_t data2;
};

// MIDI message types
#define MIDI_NOTE_OFF 0x80
#define MIDI_NOTE_ON 0x90
#define MIDI_CC 0xB0
#define MIDI_PITCH_BEND 0xE0

// Mock MIDI CC mappings from midi_config.h
#define CC_303_VOLUME       7
#define CC_303_PAN          10
#define CC_303_WAVEFORM     70
#define CC_303_RESO         71
#define CC_303_CUTOFF       74
#define CC_303_ATTACK       73
#define CC_303_DECAY        72
#define CC_303_ENVMOD_LVL   75
#define CC_303_ACCENT_LVL   76
#define CC_303_REVERB_SEND  91
#define CC_303_DELAY_SEND   92
#define CC_303_DISTORTION   94
#define CC_303_OVERDRIVE    95

class MockMidiHandler {
public:
    MockMidiHandler() : last_note(0), last_velocity(0), last_cc_number(0), 
                        last_cc_value(0), note_count(0) {}
    
    void handleNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) {
        last_note = note;
        last_velocity = velocity;
        note_count++;
    }
    
    void handleNoteOff(uint8_t channel, uint8_t note, uint8_t velocity) {
        if (last_note == note) {
            last_velocity = 0;
            note_count--;
        }
    }
    
    void handleControlChange(uint8_t channel, uint8_t cc_number, uint8_t cc_value) {
        last_cc_number = cc_number;
        last_cc_value = cc_value;
        
        // Apply CC mappings
        switch (cc_number) {
            case CC_303_CUTOFF:
                cutoff_value = cc_value;
                break;
            case CC_303_RESO:
                resonance_value = cc_value;
                break;
            case CC_303_VOLUME:
                volume_value = cc_value;
                break;
            case CC_303_PAN:
                pan_value = cc_value;
                break;
        }
    }
    
    void processMidiMessage(MockMidiMessage msg) {
        switch (msg.type & 0xF0) {
            case MIDI_NOTE_ON:
                if (msg.data2 > 0) {
                    handleNoteOn(msg.channel, msg.data1, msg.data2);
                } else {
                    handleNoteOff(msg.channel, msg.data1, msg.data2);
                }
                break;
            case MIDI_NOTE_OFF:
                handleNoteOff(msg.channel, msg.data1, msg.data2);
                break;
            case MIDI_CC:
                handleControlChange(msg.channel, msg.data1, msg.data2);
                break;
        }
    }
    
    uint8_t last_note;
    uint8_t last_velocity;
    uint8_t last_cc_number;
    uint8_t last_cc_value;
    int note_count;
    
    uint8_t cutoff_value = 64;
    uint8_t resonance_value = 0;
    uint8_t volume_value = 127;
    uint8_t pan_value = 64;
};

void test_midi_note_on() {
    MockMidiHandler handler;
    MockMidiMessage msg = {MIDI_NOTE_ON, 1, 60, 100}; // Note on, channel 1, C4, velocity 100
    
    handler.processMidiMessage(msg);
    
    TEST_ASSERT_EQUAL(60, handler.last_note);
    TEST_ASSERT_EQUAL(100, handler.last_velocity);
    TEST_ASSERT_EQUAL(1, handler.note_count);
}

void test_midi_note_off() {
    MockMidiHandler handler;
    
    // First, turn on a note
    MockMidiMessage note_on = {MIDI_NOTE_ON, 1, 60, 100};
    handler.processMidiMessage(note_on);
    
    // Then turn it off
    MockMidiMessage note_off = {MIDI_NOTE_OFF, 1, 60, 0};
    handler.processMidiMessage(note_off);
    
    TEST_ASSERT_EQUAL(0, handler.last_velocity);
    TEST_ASSERT_EQUAL(0, handler.note_count);
}

void test_midi_note_on_zero_velocity() {
    MockMidiHandler handler;
    
    // Note on with zero velocity should be treated as note off
    MockMidiMessage msg = {MIDI_NOTE_ON, 1, 60, 0};
    handler.processMidiMessage(msg);
    
    TEST_ASSERT_EQUAL(0, handler.last_velocity);
    TEST_ASSERT_EQUAL(0, handler.note_count);
}

void test_midi_control_change_cutoff() {
    MockMidiHandler handler;
    MockMidiMessage msg = {MIDI_CC, 1, CC_303_CUTOFF, 100};
    
    handler.processMidiMessage(msg);
    
    TEST_ASSERT_EQUAL(CC_303_CUTOFF, handler.last_cc_number);
    TEST_ASSERT_EQUAL(100, handler.last_cc_value);
    TEST_ASSERT_EQUAL(100, handler.cutoff_value);
}

void test_midi_control_change_resonance() {
    MockMidiHandler handler;
    MockMidiMessage msg = {MIDI_CC, 1, CC_303_RESO, 80};
    
    handler.processMidiMessage(msg);
    
    TEST_ASSERT_EQUAL(80, handler.resonance_value);
}

void test_midi_control_change_volume() {
    MockMidiHandler handler;
    MockMidiMessage msg = {MIDI_CC, 1, CC_303_VOLUME, 64};
    
    handler.processMidiMessage(msg);
    
    TEST_ASSERT_EQUAL(64, handler.volume_value);
}

void test_midi_control_change_pan() {
    MockMidiHandler handler;
    MockMidiMessage msg = {MIDI_CC, 1, CC_303_PAN, 32};
    
    handler.processMidiMessage(msg);
    
    TEST_ASSERT_EQUAL(32, handler.pan_value);
}

void test_midi_channel_filtering() {
    MockMidiHandler handler;
    
    // Test that messages on different channels can be distinguished
    MockMidiMessage ch1_msg = {MIDI_NOTE_ON, 1, 60, 100};
    MockMidiMessage ch2_msg = {MIDI_NOTE_ON, 2, 62, 110};
    
    handler.processMidiMessage(ch1_msg);
    TEST_ASSERT_EQUAL(60, handler.last_note);
    
    handler.processMidiMessage(ch2_msg);
    TEST_ASSERT_EQUAL(62, handler.last_note);
}

void test_midi_velocity_range() {
    MockMidiHandler handler;
    
    // Test minimum velocity
    MockMidiMessage min_vel = {MIDI_NOTE_ON, 1, 60, 1};
    handler.processMidiMessage(min_vel);
    TEST_ASSERT_EQUAL(1, handler.last_velocity);
    
    // Test maximum velocity
    MockMidiMessage max_vel = {MIDI_NOTE_ON, 1, 61, 127};
    handler.processMidiMessage(max_vel);
    TEST_ASSERT_EQUAL(127, handler.last_velocity);
}

void test_midi_cc_value_range() {
    MockMidiHandler handler;
    
    // Test minimum CC value
    MockMidiMessage min_cc = {MIDI_CC, 1, CC_303_CUTOFF, 0};
    handler.processMidiMessage(min_cc);
    TEST_ASSERT_EQUAL(0, handler.cutoff_value);
    
    // Test maximum CC value
    MockMidiMessage max_cc = {MIDI_CC, 1, CC_303_CUTOFF, 127};
    handler.processMidiMessage(max_cc);
    TEST_ASSERT_EQUAL(127, handler.cutoff_value);
}

void test_midi_note_range() {
    MockMidiHandler handler;
    
    // Test lowest MIDI note
    MockMidiMessage low_note = {MIDI_NOTE_ON, 1, 0, 100};
    handler.processMidiMessage(low_note);
    TEST_ASSERT_EQUAL(0, handler.last_note);
    
    // Test highest MIDI note
    MockMidiMessage high_note = {MIDI_NOTE_ON, 1, 127, 100};
    handler.processMidiMessage(high_note);
    TEST_ASSERT_EQUAL(127, handler.last_note);
}

void test_midi_multiple_notes() {
    MockMidiHandler handler;
    
    // Turn on multiple notes
    MockMidiMessage note1 = {MIDI_NOTE_ON, 1, 60, 100};
    MockMidiMessage note2 = {MIDI_NOTE_ON, 1, 64, 110};
    MockMidiMessage note3 = {MIDI_NOTE_ON, 1, 67, 120};
    
    handler.processMidiMessage(note1);
    TEST_ASSERT_EQUAL(1, handler.note_count);
    
    handler.processMidiMessage(note2);
    TEST_ASSERT_EQUAL(2, handler.note_count);
    
    handler.processMidiMessage(note3);
    TEST_ASSERT_EQUAL(3, handler.note_count);
    
    // Turn off one note
    MockMidiMessage note_off = {MIDI_NOTE_OFF, 1, 64, 0};
    handler.processMidiMessage(note_off);
    TEST_ASSERT_EQUAL(2, handler.note_count);
}

void test_midi_cc_to_float_conversion() {
    // Test converting MIDI CC values (0-127) to float (0.0-1.0)
    auto cc_to_float = [](uint8_t cc_value) -> float {
        return (float)cc_value / 127.0f;
    };
    
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, cc_to_float(0));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.5f, cc_to_float(64));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 1.0f, cc_to_float(127));
}

void test_midi_frequency_conversion() {
    // Test MIDI note to frequency conversion
    auto midi_to_freq = [](uint8_t note) -> float {
        return 440.0f * pow(2.0f, (note - 69) / 12.0f);
    };
    
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 440.0f, midi_to_freq(69)); // A4
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 261.63f, midi_to_freq(60)); // C4
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 880.0f, midi_to_freq(81)); // A5
}

void run_midi_tests() {
    RUN_TEST(test_midi_note_on);
    RUN_TEST(test_midi_note_off);
    RUN_TEST(test_midi_note_on_zero_velocity);
    RUN_TEST(test_midi_control_change_cutoff);
    RUN_TEST(test_midi_control_change_resonance);
    RUN_TEST(test_midi_control_change_volume);
    RUN_TEST(test_midi_control_change_pan);
    RUN_TEST(test_midi_channel_filtering);
    RUN_TEST(test_midi_velocity_range);
    RUN_TEST(test_midi_cc_value_range);
    RUN_TEST(test_midi_note_range);
    RUN_TEST(test_midi_multiple_notes);
    RUN_TEST(test_midi_cc_to_float_conversion);
    RUN_TEST(test_midi_frequency_conversion);
}

#endif