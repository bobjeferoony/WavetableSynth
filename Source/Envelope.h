#pragma once
#include "JuceHeader.h"

class Envelope
{
public:
    Envelope();
    ~Envelope() = default;

    void prepare(double sampleRate);
    float process();
    void noteOn();
    void noteOff();
    void reset();

    void setAttack(float attackMs);
    void setDecay(float decayMs);
    void setSustain(float sustainLevel);
    void setRelease(float releaseMs);

    bool isActive() const;

private:
    juce::ADSR adsr;
    juce::ADSR::Parameters params;
    bool active = false;
};
