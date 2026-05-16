#pragma once
#include "JuceHeader.h"

class Filter
{
public:
    using Type = juce::dsp::StateVariableTPTFilter<float>::Type;

    Filter();
    ~Filter() = default;

    void prepare(double sampleRate);
    void process(juce::AudioBuffer<float>& buffer, int numSamples);
    void setType(Type newType);
    void setCutoff(float newCutoffHz);
    void setResonance(float newResonance);
    void reset();

    float getCutoff() const { return cutoff; }

private:
    juce::dsp::StateVariableTPTFilter<float> svf;
    double sampleRate = 44100.0;
    Type type = Type::lowPass;
    float cutoff = 1000.0f;
    float resonance = 0.0f;
};
