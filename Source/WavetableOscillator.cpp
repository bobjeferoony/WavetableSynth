#include "WavetableOscillator.h"
#include <cmath>

WavetableOscillator::WavetableOscillator()
{
    tables = generateWavetables();
}

void WavetableOscillator::prepare(double sr)
{
    sampleRate = sr;
    phase = 0.0;
}

float WavetableOscillator::process(float position, float detuneOffset)
{
    position = juce::jlimit(0.0f, 1.0f, position);

    float tableIndexFloat = position * (numTables - 1);

    phase += (1.0 + detuneOffset) / sampleRate;
    if (phase >= 1.0)
        phase -= 1.0;

    return getInterpolatedSample(tableIndexFloat, static_cast<float>(phase));
}

void WavetableOscillator::reset()
{
    phase = 0.0;
}

float WavetableOscillator::getInterpolatedSample(float tableIndex, float phasePos) const
{
    int tableA = static_cast<int>(tableIndex);
    int tableB = juce::jmin(tableA + 1, numTables - 1);
    float frac = tableIndex - tableA;

    float phasePosFloat = phasePos * (tableSize - 1);
    int idxA = static_cast<int>(phasePosFloat);
    int idxB = juce::jmin(idxA + 1, tableSize - 1);
    float phaseFrac = phasePosFloat - idxA;

    float sampA = tables[tableA][idxA] + (tables[tableA][idxB] - tables[tableA][idxA]) * phaseFrac;
    float sampB = tables[tableB][idxA] + (tables[tableB][idxB] - tables[tableB][idxA]) * phaseFrac;

    return sampA + (sampB - sampA) * frac;
}

std::vector<std::vector<float>> WavetableOscillator::generateWavetables()
{
    std::vector<std::vector<float>> wt(numTables, std::vector<float>(tableSize, 0.0f));

    for (int t = 0; t < numTables; ++t)
    {
        float morph = static_cast<float>(t) / (numTables - 1);
        auto& table = wt[t];

        for (int i = 0; i < tableSize; ++i)
        {
            float p = static_cast<float>(i) / tableSize;
            float twoPi = 2.0f * juce::MathConstants<float>::pi;
            float sineVal = std::sin(twoPi * p);
            float sawVal = 2.0f * p - 1.0f;
            float sqrVal = (p < 0.5f) ? 1.0f : -1.0f;
            float triVal = 4.0f * std::abs(p - 0.5f) - 1.0f;

            float formant1 = 0.0f;
            {
                float sum = 0.0f;
                for (int h = 1; h <= 12; ++h)
                {
                    float amp = (h <= 5) ? 1.0f / h : 0.0f;
                    float phaseOffset = 0.0f;
                    if (h == 2 || h == 4 || h == 5) continue;
                    sum += std::sin(twoPi * p * h) * amp;
                }
                formant1 = sum;
            }

            float formant2 = 0.0f;
            {
                float sum = 0.0f;
                for (int h = 1; h <= 12; ++h)
                {
                    float amp = (h >= 3 && h <= 7) ? 1.0f / (h - 2) : 0.0f;
                    sum += std::sin(twoPi * p * h) * amp;
                }
                formant2 = sum;
            }

            float out = 0.0f;
            if (t < 3)
            {
                float m = t / 2.0f;
                out = sineVal * (1.0f - m) + sawVal * m;
            }
            else if (t < 6)
            {
                float m = (t - 3) / 2.0f;
                out = sawVal * (1.0f - m) + sqrVal * m;
            }
            else if (t < 9)
            {
                float m = (t - 6) / 2.0f;
                out = sqrVal * (1.0f - m) + triVal * m;
            }
            else if (t < 12)
            {
                float m = (t - 9) / 2.0f;
                out = triVal * (1.0f - m) + formant1 * m;
            }
            else
            {
                float m = (t - 12) / 3.0f;
                out = formant1 * (1.0f - m) + formant2 * m;
            }

            table[i] = out * 0.5f;
        }
    }

    return wt;
}
