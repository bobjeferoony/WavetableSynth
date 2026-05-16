# WavetableSynth - VST3 Wavetable Synthesizer

A polyphonic wavetable synthesizer plugin built with JUCE (VST3 format).

## Features
- 1 polyphonic wavetable oscillator with 16 tables (sine, saw, square, triangle, formant)
- Wavetable position control with linear interpolation between tables
- Unison mode (1-8 voices) with detune
- ADSR amplitude envelope with interactive editor
- Multimode state-variable filter (LP/HP/BP)
- 2 LFOs assignable to wavetable position, filter cutoff, and gain
- 4 macro knobs
- Full MIDI input with velocity sensitivity
- AudioProcessorValueTreeState preset saving
- Resizable dark futuristic GUI with piano keyboard

## Project Structure
```
WavetableSynth/
├── CMakeLists.txt
├── README.md
└── Source/
    ├── PluginProcessor.h/.cpp   - Main audio processor, APVTS, synth management
    ├── PluginEditor.h/.cpp      - GUI with all panels, sliders, displays
    ├── SynthVoice.h/.cpp        - Per-voice audio rendering
    ├── SynthSound.h/.cpp         - Midi sound (accepts all notes)
    ├── WavetableOscillator.h/.cpp - Wavetable generation and playback
    ├── Filter.h/.cpp            - Multimode state-variable filter wrapper
    ├── Envelope.h/.cpp          - ADSR envelope (wraps JUCE ADSR)
    ├── ModMatrix.h/.cpp         - Modulation routing matrix
    └── LookAndFeel.h/.cpp       - Custom dark cyan UI theme
```

## Building

### Prerequisites
- **JUCE 8** (or 7+) - Download from https://juce.com
- **CMake 3.24+**
- **C++17 capable compiler**
  - Xcode 15+ (macOS)
  - Visual Studio 2022 (Windows)

### Setup
1. Place JUCE in the project directory:
   ```
   WavetableSynth/
   ├── JUCE/          <-- Clone or unzip JUCE here
   ├── CMakeLists.txt
   └── Source/
   ```
   Or set `JUCE_DIR` when configuring CMake:
   ```
   cmake -B Build -DJUCE_DIR=/path/to/JUCE
   ```

### macOS / Xcode
```bash
cd WavetableSynth
cmake -B Build -G Xcode
open Build/WavetableSynth.xcodeproj
# Select the "WavetableSynth_VST3" scheme and build
# The .vst3 will be in Build/WavetableSynth_artefacts/VST3/
```

### Windows / Visual Studio
```powershell
cd WavetableSynth
cmake -B Build -G "Visual Studio 17 2022"
# Open Build/WavetableSynth.sln
# Build the "WavetableSynth_VST3" target
# The .vst3 will be in Build/WavetableSynth_artefacts\VST3\
```

### Standalone (optional)
Add `Standalone` to FORMATS in CMakeLists.txt to build a standalone app:
```
FORMATS VST3 AU AAX Standalone
```

## Modulation Routing

The modulation matrix routes **Sources** to **Destinations** with per-route amount controls:

### Sources
| Source     | Behaviour |
|-----------|-----------|
| **LFO1**  | Per-voice LFO, retriggers on note-on |
| **LFO2**  | Per-voice LFO, retriggers on note-on |
| **Envelope** | ADSR envelope (0→1 during note, then back to 0) |
| **Macro1-4** | Global knob values (0–1) |

### Destinations
| Destination   | Effect |
|--------------|--------|
| **Wavetable Pos** | Offset added to the base wavetable position (modulates the table index) |
| **Filter Cutoff** | Offset added to filter cutoff frequency (scaled by ±5000 Hz) |
| **Gain** | Offset added to master gain level |

### How It Works
1. Each voice maintains its own LFO phase and envelope state.
2. Per audio sample, the voice computes:
   - `lfoValue = LFO(phase, shape)`  (-1 to 1)
   - `envValue = ADSR()`  (0 to 1)
3. For each destination, modulation accumulates:
   ```
   modulation[dest] = SUM(sourceValue * amount[src][dest])
   ```
4. The effective parameter value becomes:
   ```
   effectiveValue = baseParam + modulation[dest]
   ```
5. Clamping ensures values stay within valid ranges.

### Fixed Routings (APVTS parameters)
- **LFO1 → WT Pos** (`lfo1_wt_amt`)
- **LFO1 → Filter** (`lfo1_fil_amt`)
- **LFO1 → Gain** (`lfo1_gain_amt`)
- **LFO2 → WT Pos** (`lfo2_wt_amt`)
- **LFO2 → Filter** (`lfo2_fil_amt`)
- **LFO2 → Gain** (`lfo2_gain_amt`)
- **Env → Filter** (`env_fil_amt`)

## Wavetable Bank
16 generated wavetables (2048 samples each):
- Tables 0-2: Sine → Saw morph
- Tables 3-5: Saw → Square morph
- Tables 6-8: Square → Triangle morph
- Tables 9-11: Triangle → Formant 1 morph
- Tables 12-15: Formant 1 → Formant 2 morph

Position 0 = sine, 0.5 = square-ish, 1.0 = formant vowel.

## License
This project is provided as a learning resource. All code is original.
