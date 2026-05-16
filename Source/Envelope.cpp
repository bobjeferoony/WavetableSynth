#include "Envelope.h"

Envelope::Envelope()
{
    params.attack = 0.01f;
    params.decay = 0.1f;
    params.sustain = 0.8f;
    params.release = 0.2f;
    adsr.setParameters(params);
}

void Envelope::prepare(double sampleRate)
{
    adsr.setSampleRate(sampleRate);
    adsr.reset();
}

float Envelope::process()
{
    float envVal = adsr.getNextSample();
    active = adsr.isActive();
    return envVal;
}

void Envelope::noteOn()
{
    adsr.noteOn();
    active = true;
}

void Envelope::noteOff()
{
    adsr.noteOff();
}

void Envelope::reset()
{
    adsr.reset();
}

void Envelope::setAttack(float attackMs)
{
    params.attack = juce::jlimit(0.001f, 10.0f, attackMs);
    adsr.setParameters(params);
}

void Envelope::setDecay(float decayMs)
{
    params.decay = juce::jlimit(0.001f, 10.0f, decayMs);
    adsr.setParameters(params);
}

void Envelope::setSustain(float sustainLevel)
{
    params.sustain = juce::jlimit(0.0f, 1.0f, sustainLevel);
    adsr.setParameters(params);
}

void Envelope::setRelease(float releaseMs)
{
    params.release = juce::jlimit(0.001f, 10.0f, releaseMs);
    adsr.setParameters(params);
}

bool Envelope::isActive() const
{
    return adsr.isActive();
}
