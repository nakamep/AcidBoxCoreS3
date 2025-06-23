#ifndef TEST_SAMPLER_H
#define TEST_SAMPLER_H

#include <unity.h>
#include <Arduino.h>

// Mock sample data structure
struct MockSampleInfo {
    float* data;
    uint32_t length;
    uint16_t sample_rate;
    bool is_loaded;
    float volume;
    float pan;
    float pitch;
    uint8_t decay_midi;
    uint8_t volume_midi;
    uint8_t pan_midi;
    uint8_t pitch_midi;
};

class MockSampler {
public:
    MockSampler() : _volume(1.0f), _sendReverb(0.0f), _sendDelay(0.0f), 
                    selectedNote(0), sampleInfoCount(12) {
        // Initialize mock samples
        for (int i = 0; i < 12; i++) {
            samples[i].data = nullptr;
            samples[i].length = 1000; // Mock length
            samples[i].sample_rate = 44100;
            samples[i].is_loaded = true;
            samples[i].volume = 1.0f;
            samples[i].pan = 0.5f;
            samples[i].pitch = 1.0f;
            samples[i].decay_midi = 64;
            samples[i].volume_midi = 127;
            samples[i].pan_midi = 64;
            samples[i].pitch_midi = 64;
            
            playback_active[i] = false;
            playback_position[i] = 0;
        }
    }
    
    void Init() {
        _volume = 1.0f;
        _sendReverb = 0.0f;
        _sendDelay = 0.0f;
        selectedNote = 0;
    }
    
    void SelectNote(uint8_t note) {
        selectedNote = note % sampleInfoCount;
    }
    
    void SetNotePan_Midi(uint8_t data1) {
        if (selectedNote < sampleInfoCount) {
            samples[selectedNote].pan_midi = data1;
            samples[selectedNote].pan = (float)data1 / 127.0f;
        }
    }
    
    void SetNoteDecay_Midi(uint8_t data1) {
        if (selectedNote < sampleInfoCount) {
            samples[selectedNote].decay_midi = data1;
        }
    }
    
    void SetNoteVolume_Midi(uint8_t data1) {
        if (selectedNote < sampleInfoCount) {
            samples[selectedNote].volume_midi = data1;
            samples[selectedNote].volume = (float)data1 / 127.0f;
        }
    }
    
    void SetSoundPitch_Midi(uint8_t data1) {
        if (selectedNote < sampleInfoCount) {
            samples[selectedNote].pitch_midi = data1;
            // Convert MIDI value to pitch ratio (0.5x to 2x)
            samples[selectedNote].pitch = 0.5f + ((float)data1 / 127.0f) * 1.5f;
        }
    }
    
    void SetSoundPitch(float value) {
        if (selectedNote < sampleInfoCount) {
            samples[selectedNote].pitch = value;
        }
    }
    
    void SetDelaySend(uint8_t lvl) {
        _sendDelay = (float)lvl / 127.0f;
    }
    
    void SetReverbSend(uint8_t lvl) {
        _sendReverb = (float)lvl / 127.0f;
    }
    
    void SetVolume(float value) {
        _volume = value;
    }
    
    void NoteOn(uint8_t note, uint8_t vol) {
        uint8_t sample_idx = note % sampleInfoCount;
        if (sample_idx < sampleInfoCount && samples[sample_idx].is_loaded) {
            playback_active[sample_idx] = true;
            playback_position[sample_idx] = 0;
            playback_velocity[sample_idx] = vol;
        }
    }
    
    void NoteOff(uint8_t note) {
        uint8_t sample_idx = note % sampleInfoCount;
        if (sample_idx < sampleInfoCount) {
            playback_active[sample_idx] = false;
        }
    }
    
    void Process(float* left, float* right) {
        *left = 0.0f;
        *right = 0.0f;
        
        for (int i = 0; i < sampleInfoCount; i++) {
            if (playback_active[i] && samples[i].is_loaded) {
                // Simple mock playback
                float sample_value = sin(2.0f * PI * playback_position[i] / 100.0f);
                sample_value *= samples[i].volume * _volume;
                sample_value *= (float)playback_velocity[i] / 127.0f;
                
                // Apply panning
                float pan = samples[i].pan;
                *left += sample_value * (1.0f - pan);
                *right += sample_value * pan;
                
                // Advance playback position
                playback_position[i] += samples[i].pitch;
                
                // Stop when sample ends
                if (playback_position[i] >= samples[i].length) {
                    playback_active[i] = false;
                }
            }
        }
    }
    
    uint16_t GetSoundSamplerate() {
        return selectedNote < sampleInfoCount ? samples[selectedNote].sample_rate : 44100;
    }
    
    uint8_t GetSoundDecay_Midi() {
        return selectedNote < sampleInfoCount ? samples[selectedNote].decay_midi : 64;
    }
    
    uint16_t GetSoundPan_Midi() {
        return selectedNote < sampleInfoCount ? samples[selectedNote].pan_midi : 64;
    }
    
    uint8_t GetSoundPitch_Midi() {
        return selectedNote < sampleInfoCount ? samples[selectedNote].pitch_midi : 64;
    }
    
    uint8_t GetSoundVolume_Midi() {
        return selectedNote < sampleInfoCount ? samples[selectedNote].volume_midi : 127;
    }
    
    int32_t GetSamplesCount() {
        return sampleInfoCount;
    }
    
    bool IsSampleActive(uint8_t note) {
        uint8_t idx = note % sampleInfoCount;
        return idx < sampleInfoCount ? playback_active[idx] : false;
    }

private:
    static const int MAX_SAMPLES = 12;
    MockSampleInfo samples[MAX_SAMPLES];
    bool playback_active[MAX_SAMPLES];
    float playback_position[MAX_SAMPLES];
    uint8_t playback_velocity[MAX_SAMPLES];
    
    float _volume;
    float _sendReverb;
    float _sendDelay;
    uint8_t selectedNote;
    int sampleInfoCount;
};

void test_sampler_init() {
    MockSampler sampler;
    sampler.Init();
    
    TEST_ASSERT_EQUAL(12, sampler.GetSamplesCount());
    TEST_ASSERT_EQUAL(0, sampler.GetSoundPan_Midi()); // selectedNote should be 0
}

void test_sampler_note_selection() {
    MockSampler sampler;
    sampler.Init();
    
    sampler.SelectNote(5);
    TEST_ASSERT_EQUAL(44100, sampler.GetSoundSamplerate()); // Should access sample 5
    
    sampler.SelectNote(15); // Should wrap around to 3 (15 % 12)
    TEST_ASSERT_EQUAL(44100, sampler.GetSoundSamplerate());
}

void test_sampler_note_on_off() {
    MockSampler sampler;
    sampler.Init();
    
    // Note on
    sampler.NoteOn(36, 100); // Kick drum typically
    TEST_ASSERT_TRUE(sampler.IsSampleActive(36));
    
    // Note off
    sampler.NoteOff(36);
    TEST_ASSERT_FALSE(sampler.IsSampleActive(36));
}

void test_sampler_multiple_notes() {
    MockSampler sampler;
    sampler.Init();
    
    // Play multiple samples simultaneously
    sampler.NoteOn(36, 100); // Kick
    sampler.NoteOn(38, 80);  // Snare
    sampler.NoteOn(42, 60);  // Hi-hat
    
    TEST_ASSERT_TRUE(sampler.IsSampleActive(36));
    TEST_ASSERT_TRUE(sampler.IsSampleActive(38));
    TEST_ASSERT_TRUE(sampler.IsSampleActive(42));
    
    // Stop one sample
    sampler.NoteOff(38);
    TEST_ASSERT_TRUE(sampler.IsSampleActive(36));
    TEST_ASSERT_FALSE(sampler.IsSampleActive(38));
    TEST_ASSERT_TRUE(sampler.IsSampleActive(42));
}

void test_sampler_volume_control() {
    MockSampler sampler;
    sampler.Init();
    
    sampler.SelectNote(0);
    sampler.SetNoteVolume_Midi(64); // Half volume
    
    TEST_ASSERT_EQUAL(64, sampler.GetSoundVolume_Midi());
    
    sampler.SetNoteVolume_Midi(127); // Full volume
    TEST_ASSERT_EQUAL(127, sampler.GetSoundVolume_Midi());
}

void test_sampler_pan_control() {
    MockSampler sampler;
    sampler.Init();
    
    sampler.SelectNote(0);
    sampler.SetNotePan_Midi(0); // Full left
    TEST_ASSERT_EQUAL(0, sampler.GetSoundPan_Midi());
    
    sampler.SetNotePan_Midi(127); // Full right
    TEST_ASSERT_EQUAL(127, sampler.GetSoundPan_Midi());
    
    sampler.SetNotePan_Midi(64); // Center
    TEST_ASSERT_EQUAL(64, sampler.GetSoundPan_Midi());
}

void test_sampler_pitch_control() {
    MockSampler sampler;
    sampler.Init();
    
    sampler.SelectNote(0);
    sampler.SetSoundPitch_Midi(64); // Normal pitch
    TEST_ASSERT_EQUAL(64, sampler.GetSoundPitch_Midi());
    
    sampler.SetSoundPitch_Midi(0); // Low pitch
    TEST_ASSERT_EQUAL(0, sampler.GetSoundPitch_Midi());
    
    sampler.SetSoundPitch_Midi(127); // High pitch
    TEST_ASSERT_EQUAL(127, sampler.GetSoundPitch_Midi());
}

void test_sampler_decay_control() {
    MockSampler sampler;
    sampler.Init();
    
    sampler.SelectNote(0);
    sampler.SetNoteDecay_Midi(32); // Short decay
    TEST_ASSERT_EQUAL(32, sampler.GetSoundDecay_Midi());
    
    sampler.SetNoteDecay_Midi(127); // Long decay
    TEST_ASSERT_EQUAL(127, sampler.GetSoundDecay_Midi());
}

void test_sampler_effects_sends() {
    MockSampler sampler;
    sampler.Init();
    
    sampler.SetReverbSend(64);
    sampler.SetDelaySend(32);
    
    // Effects sends are internal, so we test that they don't crash
    TEST_ASSERT_TRUE(true);
}

void test_sampler_audio_output() {
    MockSampler sampler;
    sampler.Init();
    
    // Start a sample
    sampler.NoteOn(36, 127);
    
    float left, right;
    sampler.Process(&left, &right);
    
    // Should produce some output
    TEST_ASSERT_TRUE(abs(left) > 0.01f || abs(right) > 0.01f);
}

void test_sampler_stereo_panning() {
    MockSampler sampler;
    sampler.Init();
    
    // Test left panning
    sampler.SelectNote(0);
    sampler.SetNotePan_Midi(0); // Full left
    sampler.NoteOn(36, 127);
    
    float left1, right1;
    sampler.Process(&left1, &right1);
    
    // Should have more output on left channel
    TEST_ASSERT_TRUE(abs(left1) >= abs(right1));
    
    // Test right panning
    sampler.NoteOff(36);
    sampler.SetNotePan_Midi(127); // Full right
    sampler.NoteOn(36, 127);
    
    float left2, right2;
    sampler.Process(&left2, &right2);
    
    // Should have more output on right channel
    TEST_ASSERT_TRUE(abs(right2) >= abs(left2));
}

void test_sampler_velocity_response() {
    MockSampler sampler;
    sampler.Init();
    
    // Test low velocity
    sampler.NoteOn(36, 32);
    float left1, right1;
    sampler.Process(&left1, &right1);
    float low_vel_amp = abs(left1) + abs(right1);
    
    sampler.NoteOff(36);
    
    // Test high velocity
    sampler.NoteOn(36, 127);
    float left2, right2;
    sampler.Process(&left2, &right2);
    float high_vel_amp = abs(left2) + abs(right2);
    
    // Higher velocity should produce higher amplitude
    TEST_ASSERT_TRUE(high_vel_amp >= low_vel_amp);
}

void test_sampler_master_volume() {
    MockSampler sampler;
    sampler.Init();
    
    sampler.SetVolume(0.5f); // Half volume
    sampler.NoteOn(36, 127);
    
    float left, right;
    sampler.Process(&left, &right);
    
    // Should produce output but reduced by master volume
    TEST_ASSERT_TRUE(abs(left) + abs(right) > 0.01f);
    TEST_ASSERT_TRUE(abs(left) + abs(right) < 1.0f); // Should be attenuated
}

void run_sampler_tests() {
    RUN_TEST(test_sampler_init);
    RUN_TEST(test_sampler_note_selection);
    RUN_TEST(test_sampler_note_on_off);
    RUN_TEST(test_sampler_multiple_notes);
    RUN_TEST(test_sampler_volume_control);
    RUN_TEST(test_sampler_pan_control);
    RUN_TEST(test_sampler_pitch_control);
    RUN_TEST(test_sampler_decay_control);
    RUN_TEST(test_sampler_effects_sends);
    RUN_TEST(test_sampler_audio_output);
    RUN_TEST(test_sampler_stereo_panning);
    RUN_TEST(test_sampler_velocity_response);
    RUN_TEST(test_sampler_master_volume);
}

#endif