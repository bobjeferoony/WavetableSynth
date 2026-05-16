#include "ModMatrix.h"

ModMatrix::ModMatrix()
{
    for (int s = 0; s < NumSources; ++s)
        for (int d = 0; d < NumDestinations; ++d)
            amounts[s][d] = 0.0f;
}

void ModMatrix::setAmount(Source src, Destination dest, float amount)
{
    if (src >= 0 && src < NumSources && dest >= 0 && dest < NumDestinations)
        amounts[src][dest] = juce::jlimit(-1.0f, 1.0f, amount);
}

float ModMatrix::getAmount(Source src, Destination dest) const
{
    if (src >= 0 && src < NumSources && dest >= 0 && dest < NumDestinations)
        return amounts[src][dest];
    return 0.0f;
}

float ModMatrix::getModulation(Source src, Destination dest,
                               float lfo1Val, float lfo2Val,
                               float envVal, float* macroVals) const
{
    float sourceVal = 0.0f;
    switch (src)
    {
        case LFO1:      sourceVal = lfo1Val;   break;
        case LFO2:      sourceVal = lfo2Val;   break;
        case Envelope:  sourceVal = envVal;    break;
        case Macro1:    sourceVal = macroVals[0]; break;
        case Macro2:    sourceVal = macroVals[1]; break;
        case Macro3:    sourceVal = macroVals[2]; break;
        case Macro4:    sourceVal = macroVals[3]; break;
        default:        sourceVal = 0.0f;       break;
    }

    return sourceVal * getAmount(src, dest);
}

/*
== Modulation Routing Explanation ==

The ModMatrix routes modulation sources to destinations:

Sources:
  LFO1, LFO2     - Low-frequency oscillators (per-voice, retrigger on note-on)
  Envelope       - ADSR amplitude envelope (per-voice)
  Macro1-4       - Global macro knob values

Destinations:
  WavetablePos   - Wavetable position (0-1 range, modulated adds offset)
  FilterCutoff   - Filter cutoff frequency (adds bipolar offset in Hz)
  Gain           - Master gain (adds bipolar offset)

Each Source->Destination pair has an amount parameter (-1 to 1).
A positive amount means the source increases the destination value.
A negative amount means the source decreases the destination value.

In the audio render loop, the voice:
1. Computes LFO output values based on LFO phase and shape
2. Gets the envelope value
3. For each destination, sums all source contributions:
   modulation = SUM(source_val * amount[src][dest])
4. Applies modulation to the base parameter value:
   effective_value = base_param + modulation
5. Uses the effective value for audio generation

Example: LFO1 -> Filter Cutoff at +0.5 means:
  effective_cutoff = base_cutoff + (lfo1_output * 0.5 * cutoff_range)
*/
