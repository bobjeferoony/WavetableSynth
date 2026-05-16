#pragma once
#include <JuceHeader.h>

class ModMatrix
{
public:
    enum Source
    {
        LFO1 = 0,
        LFO2,
        Envelope,
        Macro1,
        Macro2,
        Macro3,
        Macro4,
        NumSources
    };

    enum Destination
    {
        WavetablePos = 0,
        FilterCutoff,
        Gain,
        NumDestinations
    };

    struct Connection
    {
        Source source;
        Destination destination;
        float amount;
    };

    ModMatrix();
    ~ModMatrix() = default;

    void setAmount(Source src, Destination dest, float amount);
    float getAmount(Source src, Destination dest) const;
    float getModulation(Source src, Destination dest,
                        float lfo1Val, float lfo2Val,
                        float envVal, float* macroVals) const;

    static constexpr int numSourceDestPairs = NumSources * NumDestinations;

private:
    float amounts[NumSources][NumDestinations];
};
