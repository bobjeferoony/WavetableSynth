#include "SynthVoice.h"
#include "SynthSound.h"
#include <cmath>

SynthVoice::SynthVoice() {}

bool SynthVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<SynthSound*>(sound) != nullptr;
}

void SynthVoice::startNote(int midiNoteNumber, float velocity,
                           juce::SynthesiserSound* /*sound*/,
                           int /*currentPitchWheelPosition*/)
{
    currentNote = midiNoteNumber;
    currentVelocity = velocity;

    oscillator.reset();
    filter.reset();
    envelope.reset();
    envelope.noteOn();

    for (auto& lfo : lfos)
        lfo.phase = 0.0;
}

void SynthVoice::stopNote(float /*velocity*/, bool allowTailOff)
{
    envelope.noteOff();
    if (!allowTailOff)
        clearCurrentNote();
}

void SynthVoice::pitchWheelMoved(int /*newPitchWheelValue*/) {}
void SynthVoice::controllerMoved(int /*controllerNumber*/, int /*newControllerValue*/) {}

void SynthVoice::prepare(double sr, int /*blockSize*/)
{
    sampleRate = sr;
    oscillator.prepare(sr);
    filter.prepare(sr);
    envelope.prepare(sr);
}

void SynthVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer,
                                 int startSample, int numSamples)
{
    if (!isVoiceActive())
        return;

    juce::AudioBuffer<float> voiceBuffer(1, numSamples);
    voiceBuffer.clear();
    float* channelData = voiceBuffer.getWritePointer(0);

    float detuneRatio = detuneAmount * 0.01f;
    float cutoffModRange = 5000.0f;
    filter.setType(static_cast<Filter::Type>(filterType));

    for (int s = 0; s < numSamples; ++s)
    {
        float lfo1Val = getLFOValue(0);
        float lfo2Val = getLFOValue(1);
        float envVal = envelope.process();

        float wtMod = modMatrix.getModulation(ModMatrix::LFO1, ModMatrix::WavetablePos,
                                              lfo1Val, lfo2Val, envVal, macros)
                    + modMatrix.getModulation(ModMatrix::LFO2, ModMatrix::WavetablePos,
                                              lfo1Val, lfo2Val, envVal, macros);

        float filterMod = modMatrix.getModulation(ModMatrix::LFO1, ModMatrix::FilterCutoff,
                                                  lfo1Val, lfo2Val, envVal, macros)
                        + modMatrix.getModulation(ModMatrix::LFO2, ModMatrix::FilterCutoff,
                                                  lfo1Val, lfo2Val, envVal, macros)
                        + modMatrix.getModulation(ModMatrix::Envelope, ModMatrix::FilterCutoff,
                                                  lfo1Val, lfo2Val, envVal, macros);

        float gainMod = modMatrix.getModulation(ModMatrix::LFO1, ModMatrix::Gain,
                                                lfo1Val, lfo2Val, envVal, macros)
                      + modMatrix.getModulation(ModMatrix::LFO2, ModMatrix::Gain,
                                                lfo1Val, lfo2Val, envVal, macros);

        float effectiveWtPos = juce::jlimit(0.0f, 1.0f, wavetablePosition + wtMod);

        float sample = 0.0f;
        for (int v = 0; v < unisonVoices; ++v)
        {
            float detuneOffset = 0.0f;
            if (unisonVoices > 1)
            {
                float voicePos = static_cast<float>(v) / (unisonVoices - 1) - 0.5f;
                detuneOffset = voicePos * detuneRatio;
            }
            sample += oscillator.process(effectiveWtPos, detuneOffset);
        }
        sample /= static_cast<float>(unisonVoices);

        float effectiveCutoff = juce::jlimit(20.0f, 20000.0f,
                                             filterCutoff + filterMod * cutoffModRange);
        filter.setCutoff(effectiveCutoff);

        float effectiveGain = juce::jlimit(0.0f, 1.0f, masterGain + gainMod);
        sample *= effectiveGain * currentVelocity;

        channelData[s] = sample;
    }

    filter.setResonance(filterResonance);
    filter.process(voiceBuffer, numSamples);

    for (int s = 0; s < numSamples; ++s)
        outputBuffer.addSample(0, startSample + s, voiceBuffer.getSample(0, s));

    if (!envelope.isActive())
        clearCurrentNote();
}

void SynthVoice::setWavetablePosition(float pos)
{
    wavetablePosition = juce::jlimit(0.0f, 1.0f, pos);
}

void SynthVoice::setUnison(int voices)
{
    unisonVoices = juce::jlimit(1, 8, voices);
}

void SynthVoice::setDetune(float detune)
{
    detuneAmount = juce::jlimit(0.0f, 1.0f, detune);
}

void SynthVoice::setBlend(float blend)
{
    blendAmount = juce::jlimit(0.0f, 1.0f, blend);
}

void SynthVoice::setFilterType(int type)
{
    filterType = juce::jlimit(0, 2, type);
}

void SynthVoice::setFilterCutoff(float cutoff)
{
    filterCutoff = cutoff;
}

void SynthVoice::setFilterResonance(float res)
{
    filterResonance = juce::jlimit(0.0f, 1.0f, res);
}

void SynthVoice::setEnvParams(float attack, float decay, float sustain, float release)
{
    envelope.setAttack(attack);
    envelope.setDecay(decay);
    envelope.setSustain(sustain);
    envelope.setRelease(release);
}

void SynthVoice::setLfoRate(int lfoIndex, float rate)
{
    if (lfoIndex >= 0 && lfoIndex < 2)
        lfos[lfoIndex].rate = juce::jlimit(0.01f, 50.0f, rate);
}

void SynthVoice::setLfoShape(int lfoIndex, int shape)
{
    if (lfoIndex >= 0 && lfoIndex < 2)
        lfos[lfoIndex].shape = juce::jlimit(0, 4, shape);
}

void SynthVoice::setModAmount(int srcIdx, int destIdx, float amount)
{
    modMatrix.setAmount(static_cast<ModMatrix::Source>(srcIdx),
                        static_cast<ModMatrix::Destination>(destIdx),
                        amount);
}

void SynthVoice::setMacro(int index, float value)
{
    if (index >= 0 && index < 4)
        macros[index] = juce::jlimit(0.0f, 1.0f, value);
}

void SynthVoice::setGain(float gain)
{
    masterGain = juce::jlimit(0.0f, 1.0f, gain);
}

float SynthVoice::getLFOValue(int lfoIndex)
{
    if (lfoIndex < 0 || lfoIndex >= 2) return 0.0f;

    auto& lfo = lfos[lfoIndex];
    lfo.phase += lfo.rate / sampleRate;
    if (lfo.phase >= 1.0)
        lfo.phase -= 1.0;

    float p = static_cast<float>(lfo.phase);
    switch (lfo.shape)
    {
        case 0: return std::sin(2.0f * juce::MathConstants<float>::pi * p);
        case 1: return 2.0f * p - 1.0f;
        case 2: return (p < 0.5f) ? 1.0f : -1.0f;
        case 3: return 4.0f * std::abs(p - 0.5f) - 1.0f;
        case 4:
        {
            static float lastVal = 0.0f;
            if (p < (lfo.rate / sampleRate))
                lastVal = static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f;
            return lastVal;
        }
        default: return 0.0f;
    }
}
