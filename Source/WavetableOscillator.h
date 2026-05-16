#pragma once
#include <JuceHeader.h>
#include <vector>

class WavetableOscillator
{
public:
    static constexpr int tableSize = 2048;
    static constexpr int numTables = 16;

    WavetableOscillator();
    ~WavetableOscillator() = default;

    void prepare(double sampleRate);
    float process(float position, float detuneOffset = 0.0f);
    void reset();

    const std::vector<std::vector<float>>& getTables() const { return tables; }
    static std::vector<std::vector<float>> generateWavetables();

private:
    double sampleRate = 44100.0;
    double phase = 0.0;
    std::vector<std::vector<float>> tables;

    float getInterpolatedSample(float tableIndex, float phasePos) const;
};
