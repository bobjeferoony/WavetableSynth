#include "Filter.h"

Filter::Filter() {}

void Filter::prepare(double sr)
{
    sampleRate = sr;
    juce::dsp::ProcessSpec spec{ sr, 512, 1 };
    svf.prepare(spec);
    needsUpdate = true;
    updateFilter();
}

void Filter::process(juce::AudioBuffer<float>& buffer, int numSamples)
{
    if (needsUpdate)
        updateFilter();

    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    svf.process(context);
}

void Filter::setType(Type newType)
{
    if (type != newType)
    {
        type = newType;
        needsUpdate = true;
    }
}

void Filter::setCutoff(float newCutoffHz)
{
    newCutoffHz = juce::jlimit(20.0f, 20000.0f, newCutoffHz);
    if (std::abs(cutoff - newCutoffHz) > 0.1f)
    {
        cutoff = newCutoffHz;
        needsUpdate = true;
    }
}

void Filter::setResonance(float newResonance)
{
    newResonance = juce::jlimit(0.0f, 1.0f, newResonance);
    if (std::abs(resonance - newResonance) > 0.001f)
    {
        resonance = newResonance;
        needsUpdate = true;
    }
}

void Filter::updateFilter()
{
    float q = 1.0f - resonance * 0.95f;
    q = juce::jmax(0.01f, q);

    switch (type)
    {
        case LowPass:
            svf.coefficients = juce::dsp::StateVariableFilter::Filter<float>::Coefficients::makeLowPass(sampleRate, cutoff, q);
            break;
        case HighPass:
            svf.coefficients = juce::dsp::StateVariableFilter::Filter<float>::Coefficients::makeHighPass(sampleRate, cutoff, q);
            break;
        case BandPass:
            svf.coefficients = juce::dsp::StateVariableFilter::Filter<float>::Coefficients::makeBandPass(sampleRate, cutoff, q);
            break;
    }

    needsUpdate = false;
}

void Filter::reset()
{
    svf.reset();
}
