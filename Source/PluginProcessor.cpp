#include "PluginProcessor.h"
#include "PluginEditor.h"

static juce::String paramWtPosition   = "wt_position";
static juce::String paramUnison       = "unison";
static juce::String paramDetune       = "detune";
static juce::String paramBlend        = "wt_blend";
static juce::String paramFilterType   = "filter_type";
static juce::String paramFilterCutoff = "filter_cutoff";
static juce::String paramFilterRes    = "filter_res";
static juce::String paramAttack       = "attack";
static juce::String paramDecay        = "decay";
static juce::String paramSustain      = "sustain";
static juce::String paramRelease      = "release";
static juce::String paramLfo1Rate     = "lfo1_rate";
static juce::String paramLfo1Shape    = "lfo1_shape";
static juce::String paramLfo1WtAmt    = "lfo1_wt_amt";
static juce::String paramLfo1FilAmt   = "lfo1_fil_amt";
static juce::String paramLfo1GainAmt  = "lfo1_gain_amt";
static juce::String paramLfo2Rate     = "lfo2_rate";
static juce::String paramLfo2Shape    = "lfo2_shape";
static juce::String paramLfo2WtAmt    = "lfo2_wt_amt";
static juce::String paramLfo2FilAmt   = "lfo2_fil_amt";
static juce::String paramLfo2GainAmt  = "lfo2_gain_amt";
static juce::String paramEnvFilAmt    = "env_fil_amt";
static juce::String paramMacro1       = "macro1";
static juce::String paramMacro2       = "macro2";
static juce::String paramMacro3       = "macro3";
static juce::String paramMacro4       = "macro4";
static juce::String paramGain         = "gain";

juce::AudioProcessorValueTreeState::ParameterLayout
WavetableSynthAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    auto floatRange = juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f);
    auto cutoffRange = juce::NormalisableRange<float>(20.0f, 20000.0f, 0.1f, 0.3f);
    auto timeRange = juce::NormalisableRange<float>(0.001f, 10.0f, 0.001f, 0.5f);
    auto rateRange = juce::NormalisableRange<float>(0.01f, 50.0f, 0.01f, 0.5f);
    auto modRange = juce::NormalisableRange<float>(-1.0f, 1.0f, 0.001f);
    auto intRange = juce::NormalisableRange<float>(1.0f, 8.0f, 1.0f);

    layout.add(std::make_unique<juce::AudioParameterFloat>(paramWtPosition, "Wavetable Position", floatRange, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterInt>(paramUnison, "Unison", 1, 8, 1));
    layout.add(std::make_unique<juce::AudioParameterFloat>(paramDetune, "Detune", floatRange, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(paramBlend, "Blend", floatRange, 0.5f));

    juce::StringArray filterTypes = { "Low-Pass", "High-Pass", "Band-Pass" };
    layout.add(std::make_unique<juce::AudioParameterChoice>(paramFilterType, "Filter Type", filterTypes, 0));
    layout.add(std::make_unique<juce::AudioParameterFloat>(paramFilterCutoff, "Filter Cutoff", cutoffRange, 1000.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(paramFilterRes, "Filter Resonance", floatRange, 0.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(paramAttack, "Attack", timeRange, 0.01f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(paramDecay, "Decay", timeRange, 0.2f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(paramSustain, "Sustain", floatRange, 0.8f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(paramRelease, "Release", timeRange, 0.3f));

    juce::StringArray lfoShapes = { "Sine", "Saw", "Square", "Triangle", "S&H" };
    layout.add(std::make_unique<juce::AudioParameterFloat>(paramLfo1Rate, "LFO1 Rate", rateRange, 1.0f));
    layout.add(std::make_unique<juce::AudioParameterChoice>(paramLfo1Shape, "LFO1 Shape", lfoShapes, 0));
    layout.add(std::make_unique<juce::AudioParameterFloat>(paramLfo1WtAmt, "LFO1->WT Pos", modRange, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(paramLfo1FilAmt, "LFO1->Filter", modRange, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(paramLfo1GainAmt, "LFO1->Gain", modRange, 0.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(paramLfo2Rate, "LFO2 Rate", rateRange, 3.0f));
    layout.add(std::make_unique<juce::AudioParameterChoice>(paramLfo2Shape, "LFO2 Shape", lfoShapes, 0));
    layout.add(std::make_unique<juce::AudioParameterFloat>(paramLfo2WtAmt, "LFO2->WT Pos", modRange, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(paramLfo2FilAmt, "LFO2->Filter", modRange, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(paramLfo2GainAmt, "LFO2->Gain", modRange, 0.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(paramEnvFilAmt, "Env->Filter", modRange, 0.5f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(paramMacro1, "Macro 1", floatRange, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(paramMacro2, "Macro 2", floatRange, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(paramMacro3, "Macro 3", floatRange, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(paramMacro4, "Macro 4", floatRange, 0.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(paramGain, "Gain", floatRange, 0.75f));

    return layout;
}

WavetableSynthAudioProcessor::WavetableSynthAudioProcessor()
    : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::mono(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout())
{
    wavetableBank = WavetableOscillator::generateWavetables();

    for (int i = 0; i < maxVoices; ++i)
        synth.addVoice(new SynthVoice());

    synth.addSound(new SynthSound());

}

WavetableSynthAudioProcessor::~WavetableSynthAudioProcessor()
{
}

void WavetableSynthAudioProcessor::prepareToPlay(double sr, int blockSize)
{
    synth.setCurrentPlaybackSampleRate(sr);

    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
            voice->prepare(sr, blockSize);
    }

    updateVoiceParams();
}

void WavetableSynthAudioProcessor::releaseResources() {}

void WavetableSynthAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                                 juce::MidiBuffer& midiMessages)
{
    buffer.clear();

    midiKeyboardState.processNextMidiBuffer(midiMessages, 0, buffer.getNumSamples(), true);

    updateVoiceParams();
    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

void WavetableSynthAudioProcessor::updateVoiceParams(int /*voiceIndex*/)
{
    auto getFloat = [&](const juce::String& id) -> float {
        return apvts.getRawParameterValue(id)->load();
    };

    float wtPos = getFloat(paramWtPosition);
    int unison = static_cast<int>(getFloat(paramUnison));
    float detune = getFloat(paramDetune);
    float blend = getFloat(paramBlend);
    int filterType = static_cast<int>(getFloat(paramFilterType));
    float filterCutoff = getFloat(paramFilterCutoff);
    float filterRes = getFloat(paramFilterRes);
    float attack = getFloat(paramAttack);
    float decay = getFloat(paramDecay);
    float sustain = getFloat(paramSustain);
    float release = getFloat(paramRelease);
    float lfo1Rate = getFloat(paramLfo1Rate);
    int lfo1Shape = static_cast<int>(getFloat(paramLfo1Shape));
    float lfo1WtAmt = getFloat(paramLfo1WtAmt);
    float lfo1FilAmt = getFloat(paramLfo1FilAmt);
    float lfo1GainAmt = getFloat(paramLfo1GainAmt);
    float lfo2Rate = getFloat(paramLfo2Rate);
    int lfo2Shape = static_cast<int>(getFloat(paramLfo2Shape));
    float lfo2WtAmt = getFloat(paramLfo2WtAmt);
    float lfo2FilAmt = getFloat(paramLfo2FilAmt);
    float lfo2GainAmt = getFloat(paramLfo2GainAmt);
    float envFilAmt = getFloat(paramEnvFilAmt);
    float macro1 = getFloat(paramMacro1);
    float macro2 = getFloat(paramMacro2);
    float macro3 = getFloat(paramMacro3);
    float macro4 = getFloat(paramMacro4);
    float gain = getFloat(paramGain);

    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
        {
            voice->setWavetablePosition(wtPos);
            voice->setUnison(unison);
            voice->setDetune(detune);
            voice->setBlend(blend);
            voice->setFilterType(filterType);
            voice->setFilterCutoff(filterCutoff);
            voice->setFilterResonance(filterRes);
            voice->setEnvParams(attack, decay, sustain, release);
            voice->setLfoRate(0, lfo1Rate);
            voice->setLfoShape(0, lfo1Shape);
            voice->setLfoRate(1, lfo2Rate);
            voice->setLfoShape(1, lfo2Shape);

            voice->setModAmount(ModMatrix::LFO1, ModMatrix::WavetablePos, lfo1WtAmt);
            voice->setModAmount(ModMatrix::LFO1, ModMatrix::FilterCutoff, lfo1FilAmt);
            voice->setModAmount(ModMatrix::LFO1, ModMatrix::Gain, lfo1GainAmt);
            voice->setModAmount(ModMatrix::LFO2, ModMatrix::WavetablePos, lfo2WtAmt);
            voice->setModAmount(ModMatrix::LFO2, ModMatrix::FilterCutoff, lfo2FilAmt);
            voice->setModAmount(ModMatrix::LFO2, ModMatrix::Gain, lfo2GainAmt);
            voice->setModAmount(ModMatrix::Envelope, ModMatrix::FilterCutoff, envFilAmt);

            voice->setMacro(0, macro1);
            voice->setMacro(1, macro2);
            voice->setMacro(2, macro3);
            voice->setMacro(3, macro4);
            voice->setGain(gain);
        }
    }
}

juce::AudioProcessorEditor* WavetableSynthAudioProcessor::createEditor()
{
    return new WavetableSynthAudioProcessorEditor(*this);
}

bool WavetableSynthAudioProcessor::hasEditor() const { return true; }

const juce::String WavetableSynthAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool WavetableSynthAudioProcessor::acceptsMidi() const { return true; }
bool WavetableSynthAudioProcessor::producesMidi() const { return false; }
bool WavetableSynthAudioProcessor::isMidiEffect() const { return false; }
double WavetableSynthAudioProcessor::getTailLengthSeconds() const { return 0.0; }

int WavetableSynthAudioProcessor::getNumPrograms() { return 1; }
int WavetableSynthAudioProcessor::getCurrentProgram() { return 0; }
void WavetableSynthAudioProcessor::setCurrentProgram(int) {}
const juce::String WavetableSynthAudioProcessor::getProgramName(int)
{
    return "Init";
}
void WavetableSynthAudioProcessor::changeProgramName(int, const juce::String&) {}

void WavetableSynthAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void WavetableSynthAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml != nullptr)
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new WavetableSynthAudioProcessor();
}
