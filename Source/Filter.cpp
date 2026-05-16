#include "Filter.h"

Filter::Filter() {}

void Filter::prepare(double sr)
{
    sampleRate = sr;
    juce::dsp::ProcessSpec spec{ sr, 512, 1 };
    svf.prepare(spec);
    svf.setCutoffFrequency(cutoff);
    svf.setResonance(resonance);
}

void Filter::process(juce::AudioBuffer<float>& buffer, int numSamples)
{
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    svf.process(context);
}

void Filter::setType(Type newType)
{
    type = newType;
    svf.setType(newType);
}

void Filter::setCutoff(float newCutoffHz)
{
    cutoff = juce::jlimit(20.0f, 20000.0f, newCutoffHz);
    svf.setCutoffFrequency(cutoff);
}

void Filter::setResonance(float newResonance)
{
    resonance = juce::jlimit(0.0f, 1.0f, newResonance);
    svf.setResonance(resonance);
}

void Filter::reset()
{
    svf.reset();
}
