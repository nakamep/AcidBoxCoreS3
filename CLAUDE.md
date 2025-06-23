# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

AcidBox is an ESP32-based audio synthesizer project that implements a combination of TB-303 and TR-808 style synthesizers. It's a headless MIDI-driven audio device that outputs 44.1kHz 16-bit stereo audio via I2S to an external DAC or 8-bit to the built-in DAC.

## Hardware Requirements

- ESP32 or ESP32-S3 with at least 4MB PSRAM (ESP32 WROVER module recommended)
- External DAC like PCM5102 (highly recommended for quality)
- Compatible with both regular ESP32 and ESP32-S3 variants

## Development Environment

This is an Arduino IDE project (tested with v1.8.20) for ESP32/ESP32-S3 microcontrollers.

### Board Configuration (Arduino IDE)
- Board: ESP32 Dev Module (or ESP32S3 Dev Module for S3)
- Partition scheme: No OTA (1MB APP/ 3MB SPIFFS)
- PSRAM: enabled (or appropriate PSRAM type)

### Dependencies
- MIDI Library: https://github.com/FortySevenEffects/arduino_midi_library

### Sample Upload Required
You must upload drum samples from the `/data` folder to ESP32 flash (LittleFS) using:
- Arduino IDE 1.x: https://github.com/lorol/LITTLEFS#arduino-esp32-littlefs-filesystem-upload-tool
- Arduino IDE 2.x: https://github.com/earlephilhower/arduino-littlefs-upload

Without samples, only the default 40kB samples from samples.h will be available.

## Architecture

### Core Components
- **Dual TB-303 Synthesizers**: Two independent acid synth voices (MIDI channels 1 & 2)
- **TR-808 Style Drums**: Drum machine (MIDI channel 10)
- **Audio Engine**: Dual-core ESP32 processing with DMA buffers
- **Effects Chain**: Reverb, delay, compression, filters, overdrive, wavefolder

### Key Files
- `AcidBox.ino`: Main application entry point and audio processing loop
- `config.h`: Hardware configuration, pin definitions, and compile-time options
- `synthvoice.h/.ino`: TB-303 synthesizer voice implementation
- `sampler.h/.ino`: TR-808 drum sampler with LittleFS sample loading
- `midi_config.h`: MIDI CC mappings for GM and vintage modes
- `fx_*.h/.ino`: Various audio effects implementations
- `general.ino`: Core audio processing and I2S output

### Audio Processing Flow
1. MIDI input triggers note events and CC changes
2. Synthesizer voices generate audio in floating-point buffers
3. Effects processing (filters, distortion, modulation)
4. Global effects (reverb, delay, compression)
5. I2S output to DAC with format conversion

### Configuration Options (config.h)
- `JUKEBOX`: Enable standalone auto-composition mode
- `USE_INTERNAL_DAC`: Use ESP32 built-in 8-bit DAC instead of external I2S DAC
- `NO_PSRAM`: Operation without PSRAM (limited features)
- `MIDI_VIA_SERIAL`: Use Hairless MIDI over USB serial @115200 baud
- `MIDI_VIA_SERIAL2`: Use standard MIDI @31250 baud on UART2

### Filter Types (synthvoice.h)
- Type 0: Moogladder by Victor Lazzarini
- Type 1: Tim Stilson's model by Aaron Krajeski
- Type 2: Open303 filter (default)

## MIDI Control

### Synthesizer Controls (per voice)
- Filter cutoff (CC 74), resonance (CC 71)
- Envelope modulation (CC 75), attack (CC 73), decay (CC 72)
- Accent level (CC 76), volume (CC 7), pan (CC 10)
- Waveform blend (CC 70), portamento (CC 65), portamento time (CC 5)
- Effects sends: reverb (CC 91), delay (CC 92)
- Distortion (CC 94), overdrive (CC 95)

### Drum Controls
- Per-instrument tone and level controls
- Global filter with HP/LP behavior (CC 74)
- Individual drum tuning and decay parameters

### Global Effects
- Delay time (CC 84), feedback (CC 85), level (CC 86)
- Reverb time (CC 87), level (CC 88)
- Master compressor (CC 93)

## Audio Features

- 44.1kHz/16-bit stereo output (22.05kHz/8-bit with internal DAC)
- Real-time audio processing on both ESP32 cores
- Multiple filter models and nonlinear processing
- Comprehensive effects chain with sends/returns architecture
- Sample-based drums with pitch/timing control
- Normalization matrices for consistent levels across parameter ranges

## Build Notes

- Requires up-to-date Arduino ESP32 core (v2.0.6 to v2.0.14 tested)
- Avoid ESP32 core versions 3.1.2, 3.1.3, and 3.2.0-RC1 (i2s/PSRAM bug)
- PSRAM is essential for full functionality (reverb, large sample sets)
- LittleFS filesystem used for drum sample storage