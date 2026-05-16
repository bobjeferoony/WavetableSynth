#pragma once
#include <JuceHeader.h>

class Filter
{
public:
    enum Type { LowPass = 0, HighPass, BandPass };

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
    Type type = LowPass;
    float cutoff = 1000.0f;
    float resonance = 0.0f;

    juce::dsp::StateVariableFilter::Filter<float> svf;
    double sampleRate = 44100.0;
    bool needsUpdate = true;

    void updateFilter();
};
