#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "LookAndFeel.h"

class WavetableDisplay : public juce::Component
{
public:
    WavetableDisplay(WavetableSynthAudioProcessor& p);
    void paint(juce::Graphics& g) override;
    void setWavetableData(const std::vector<float>* data);
    void updateFromPosition(float position);

private:
    WavetableSynthAudioProcessor& processor;
    std::vector<float> currentWave;
    WavetableLookAndFeel lnf;
};

class AdsrEditor : public juce::Component
{
public:
    AdsrEditor(WavetableSynthAudioProcessor& p,
               juce::AudioProcessorValueTreeState& apvts);
    void paint(juce::Graphics& g) override;
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void resized() override;
    void updateFromParams();

private:
    WavetableSynthAudioProcessor& processor;
    juce::AudioProcessorValueTreeState& apvts;
    WavetableLookAndFeel lnf;

    float attack = 0.01f;
    float decay = 0.2f;
    float sustain = 0.8f;
    float release = 0.3f;

    juce::Rectangle<float> getAttackRect() const;
    juce::Rectangle<float> getDecayRect() const;
    juce::Rectangle<float> getSustainRect() const;
    juce::Rectangle<float> getReleaseRect() const;

    enum Handle { None, AttackH, DecayH, SustainH, ReleaseH };
    Handle dragHandle = None;
};

class LfoDisplay : public juce::Component
{
public:
    LfoDisplay(WavetableSynthAudioProcessor& p, int lfoIndex);
    void paint(juce::Graphics& g) override;
    void updateFromParams();

private:
    WavetableSynthAudioProcessor& processor;
    int lfoIndex;
    float rate = 1.0f;
    int shape = 0;
    WavetableLookAndFeel lnf;
};

class WavetableSynthAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    WavetableSynthAudioProcessorEditor(WavetableSynthAudioProcessor&);
    ~WavetableSynthAudioProcessorEditor() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void timerCallback(int timerID) override;

private:
    WavetableSynthAudioProcessor& processor;

    WavetableLookAndFeel lnf;
    WavetableDisplay wtDisplay;
    AdsrEditor adsrEditor;
    LfoDisplay lfo1Display;
    LfoDisplay lfo2Display;

    juce::Label titleLabel;
    juce::TextButton initButton;

    juce::Slider wtPositionSlider;
    juce::Slider unisonSlider;
    juce::Slider detuneSlider;
    juce::Slider blendSlider;

    juce::Slider filterCutoffSlider;
    juce::Slider filterResSlider;
    juce::ComboBox filterTypeBox;

    juce::Slider lfo1RateSlider;
    juce::ComboBox lfo1ShapeBox;
    juce::Slider lfo1WtAmtSlider;
    juce::Slider lfo1FilAmtSlider;
    juce::Slider lfo1GainAmtSlider;

    juce::Slider lfo2RateSlider;
    juce::ComboBox lfo2ShapeBox;
    juce::Slider lfo2WtAmtSlider;
    juce::Slider lfo2FilAmtSlider;
    juce::Slider lfo2GainAmtSlider;

    juce::Slider envFilAmtSlider;

    juce::Slider macroSlider1;
    juce::Slider macroSlider2;
    juce::Slider macroSlider3;
    juce::Slider macroSlider4;

    juce::Slider gainSlider;

    juce::MidiKeyboardComponent midiKeyboard;

    using SliderAtt = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ComboAtt = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    std::unique_ptr<SliderAtt> wtPosAtt;
    std::unique_ptr<SliderAtt> unisonAtt;
    std::unique_ptr<SliderAtt> detuneAtt;
    std::unique_ptr<SliderAtt> blendAtt;
    std::unique_ptr<SliderAtt> filterCutoffAtt;
    std::unique_ptr<SliderAtt> filterResAtt;
    std::unique_ptr<ComboAtt> filterTypeAtt;
    std::unique_ptr<SliderAtt> lfo1RateAtt;
    std::unique_ptr<ComboAtt> lfo1ShapeAtt;
    std::unique_ptr<SliderAtt> lfo1WtAmtAtt;
    std::unique_ptr<SliderAtt> lfo1FilAmtAtt;
    std::unique_ptr<SliderAtt> lfo1GainAmtAtt;
    std::unique_ptr<SliderAtt> lfo2RateAtt;
    std::unique_ptr<ComboAtt> lfo2ShapeAtt;
    std::unique_ptr<SliderAtt> lfo2WtAmtAtt;
    std::unique_ptr<SliderAtt> lfo2FilAmtAtt;
    std::unique_ptr<SliderAtt> lfo2GainAmtAtt;
    std::unique_ptr<SliderAtt> envFilAmtAtt;
    std::unique_ptr<SliderAtt> macroAtt1;
    std::unique_ptr<SliderAtt> macroAtt2;
    std::unique_ptr<SliderAtt> macroAtt3;
    std::unique_ptr<SliderAtt> macroAtt4;
    std::unique_ptr<SliderAtt> gainAtt;

    void initSlider(juce::Slider& slider, const juce::String& label);
    void initCombo(juce::ComboBox& box, const juce::StringArray& items);
    void initButtonStyle(juce::TextButton& button, const juce::String& text);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WavetableSynthAudioProcessorEditor)
};
