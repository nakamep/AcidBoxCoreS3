#ifndef TEST_SYNTHVOICE_H
#define TEST_SYNTHVOICE_H

#include <unity.h>
#include <Arduino.h>

// Mock the complex includes for testing
class MockSynthVoice {
public:
    MockSynthVoice() : _volume(1.0f), _cutoff(1000.0f), _resonance(0.0f), 
                       _current_note(0), _note_active(false), _slide(false) {}
    
    void Init() {
        _volume = 1.0f;
        _cutoff = 1000.0f;
        _resonance = 0.0f;
        _current_note = 0;
        _note_active = false;
        _slide = false;
    }
    
    void on_midi_noteON(uint8_t note, uint8_t velocity) {
        _current_note = note;
        _note_active = true;
        _velocity = velocity;
    }
    
    void on_midi_noteOFF(uint8_t note, uint8_t velocity) {
        if (_current_note == note) {
            _note_active = false;
        }
    }
    
    void StopSound() {
        _note_active = false;
    }
    
    void SetSlideOn() { _slide = true; }
    void SetSlideOff() { _slide = false; }
    void SetVolume(float val) { _volume = val; }
    void SetCutoff(float val) { _cutoff = val; }
    void SetResonance(float val) { _resonance = val; }
    
    float Process() {
        if (!_note_active) return 0.0f;
        
        // Simple oscillator simulation
        float freq = 440.0f * pow(2.0f, (_current_note - 69) / 12.0f);
        static float phase = 0.0f;
        phase += 2.0f * PI * freq / 44100.0f;
        if (phase > 2.0f * PI) phase -= 2.0f * PI;
        
        float output = sin(phase) * (_velocity / 127.0f) * _volume;
        return output;
    }
    
    bool IsActive() const { return _note_active; }
    uint8_t GetCurrentNote() const { return _current_note; }
    float GetVolume() const { return _volume; }
    float GetCutoff() const { return _cutoff; }
    float GetResonance() const { return _resonance; }
    bool IsSlideEnabled() const { return _slide; }

private:
    float _volume;
    float _cutoff;
    float _resonance;
    uint8_t _current_note;
    uint8_t _velocity;
    bool _note_active;
    bool _slide;
};

void test_synthvoice_init() {
    MockSynthVoice voice;
    voice.Init();
    
    TEST_ASSERT_FALSE(voice.IsActive());
    TEST_ASSERT_EQUAL(0, voice.GetCurrentNote());
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 1.0f, voice.GetVolume());
}

void test_synthvoice_note_on() {
    MockSynthVoice voice;
    voice.Init();
    
    voice.on_midi_noteON(60, 100); // Middle C, velocity 100
    
    TEST_ASSERT_TRUE(voice.IsActive());
    TEST_ASSERT_EQUAL(60, voice.GetCurrentNote());
}

void test_synthvoice_note_off() {
    MockSynthVoice voice;
    voice.Init();
    
    voice.on_midi_noteON(60, 100);
    TEST_ASSERT_TRUE(voice.IsActive());
    
    voice.on_midi_noteOFF(60, 0);
    TEST_ASSERT_FALSE(voice.IsActive());
}

void test_synthvoice_note_off_wrong_note() {
    MockSynthVoice voice;
    voice.Init();
    
    voice.on_midi_noteON(60, 100);
    TEST_ASSERT_TRUE(voice.IsActive());
    
    // Note off for different note should not stop current note
    voice.on_midi_noteOFF(62, 0);
    TEST_ASSERT_TRUE(voice.IsActive());
}

void test_synthvoice_stop_sound() {
    MockSynthVoice voice;
    voice.Init();
    
    voice.on_midi_noteON(60, 100);
    TEST_ASSERT_TRUE(voice.IsActive());
    
    voice.StopSound();
    TEST_ASSERT_FALSE(voice.IsActive());
}

void test_synthvoice_volume_control() {
    MockSynthVoice voice;
    voice.Init();
    
    voice.SetVolume(0.5f);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.5f, voice.GetVolume());
    
    voice.SetVolume(0.0f);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, voice.GetVolume());
    
    voice.SetVolume(1.0f);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 1.0f, voice.GetVolume());
}

void test_synthvoice_filter_controls() {
    MockSynthVoice voice;
    voice.Init();
    
    voice.SetCutoff(2000.0f);
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 2000.0f, voice.GetCutoff());
    
    voice.SetResonance(0.7f);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.7f, voice.GetResonance());
}

void test_synthvoice_slide_control() {
    MockSynthVoice voice;
    voice.Init();
    
    TEST_ASSERT_FALSE(voice.IsSlideEnabled());
    
    voice.SetSlideOn();
    TEST_ASSERT_TRUE(voice.IsSlideEnabled());
    
    voice.SetSlideOff();
    TEST_ASSERT_FALSE(voice.IsSlideEnabled());
}

void test_synthvoice_audio_output() {
    MockSynthVoice voice;
    voice.Init();
    
    // No output when inactive
    float output = voice.Process();
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, output);
    
    // Should produce output when active
    voice.on_midi_noteON(69, 127); // A4 at full velocity
    
    float sum = 0.0f;
    int samples = 100;
    for (int i = 0; i < samples; i++) {
        float sample = voice.Process();
        sum += abs(sample);
    }
    
    float average_amplitude = sum / samples;
    TEST_ASSERT_TRUE(average_amplitude > 0.1f); // Should produce significant output
}

void test_synthvoice_velocity_response() {
    MockSynthVoice voice;
    voice.Init();
    
    // Test low velocity
    voice.on_midi_noteON(60, 32);
    float low_vel_sum = 0.0f;
    for (int i = 0; i < 50; i++) {
        low_vel_sum += abs(voice.Process());
    }
    
    voice.StopSound();
    
    // Test high velocity
    voice.on_midi_noteON(60, 127);
    float high_vel_sum = 0.0f;
    for (int i = 0; i < 50; i++) {
        high_vel_sum += abs(voice.Process());
    }
    
    // Higher velocity should produce higher amplitude
    TEST_ASSERT_TRUE(high_vel_sum > low_vel_sum);
}

void test_synthvoice_frequency_accuracy() {
    MockSynthVoice voice;
    voice.Init();
    
    // Test A4 (440Hz)
    voice.on_midi_noteON(69, 100);
    
    // Measure zero crossings to estimate frequency
    int zero_crossings = 0;
    float prev_sample = 0.0f;
    int samples = 4410; // 0.1 seconds at 44.1kHz
    
    for (int i = 0; i < samples; i++) {
        float sample = voice.Process();
        if ((prev_sample >= 0.0f && sample < 0.0f) || 
            (prev_sample < 0.0f && sample >= 0.0f)) {
            zero_crossings++;
        }
        prev_sample = sample;
    }
    
    float estimated_freq = (zero_crossings / 2.0f) * 10.0f; // Hz
    TEST_ASSERT_FLOAT_WITHIN(10.0f, 440.0f, estimated_freq); // Within 10Hz tolerance
}

void run_synthvoice_tests() {
    RUN_TEST(test_synthvoice_init);
    RUN_TEST(test_synthvoice_note_on);
    RUN_TEST(test_synthvoice_note_off);
    RUN_TEST(test_synthvoice_note_off_wrong_note);
    RUN_TEST(test_synthvoice_stop_sound);
    RUN_TEST(test_synthvoice_volume_control);
    RUN_TEST(test_synthvoice_filter_controls);
    RUN_TEST(test_synthvoice_slide_control);
    RUN_TEST(test_synthvoice_audio_output);
    RUN_TEST(test_synthvoice_velocity_response);
    RUN_TEST(test_synthvoice_frequency_accuracy);
}

#endif