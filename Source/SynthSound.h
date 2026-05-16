#pragma once
#include <JuceHeader.h>

class SynthSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote(int /*midiNote*/) override { return true; }
    bool appliesToChannel(int /*midiChannel*/) override { return true; }
};
