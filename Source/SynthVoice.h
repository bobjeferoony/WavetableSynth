#pragma once
#include <JuceHeader.h>
#include "WavetableOscillator.h"
#include "Filter.h"
#include "Envelope.h"
#include "ModMatrix.h"

class SynthVoice : public juce::SynthesiserVoice
{
public:
    SynthVoice();
    ~SynthVoice() = default;

    bool canPlaySound(juce::SynthesiserSound* sound) override;
    void startNote(int midiNoteNumber, float velocity,
                   juce::SynthesiserSound* sound,
                   int currentPitchWheelPosition) override;
    void stopNote(float velocity, bool allowTailOff) override;
    void pitchWheelMoved(int newPitchWheelValue) override;
    void controllerMoved(int controllerNumber, int newControllerValue) override;
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer,
                         int startSample, int numSamples) override;

    void prepare(double sampleRate, int blockSize);

    void setWavetablePosition(float pos);
    void setUnison(int voices);
    void setDetune(float detune);
    void setBlend(float blend);
    void setFilterType(int type);
    void setFilterCutoff(float cutoff);
    void setFilterResonance(float res);
    void setEnvParams(float attack, float decay, float sustain, float release);
    void setLfoRate(int lfoIndex, float rate);
    void setLfoShape(int lfoIndex, int shape);
    void setModAmount(int srcIdx, int destIdx, float amount);
    void setMacro(int index, float value);
    void setGain(float gain);

    const WavetableOscillator& getOscillator() const { return oscillator; }

private:
    WavetableOscillator oscillator;
    Filter filter;
    Envelope envelope;
    ModMatrix modMatrix;

    float wavetablePosition = 0.0f;
    int unisonVoices = 1;
    float detuneAmount = 0.0f;
    float blendAmount = 0.0f;
    int filterType = 0;
    float filterCutoff = 1000.0f;
    float filterResonance = 0.0f;
    float masterGain = 0.5f;
    float macros[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

    struct LFO
    {
        double phase = 0.0;
        float rate = 1.0f;
        int shape = 0;
        float process(float sampleRate);
    };

    LFO lfos[2];
    double sampleRate = 44100.0;
    float currentVelocity = 1.0f;
    int currentNote = 60;

    float getLFOValue(int lfoIndex);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SynthVoice)
};
