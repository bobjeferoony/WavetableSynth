#pragma once
#include "JuceHeader.h"

class WavetableLookAndFeel : public juce::LookAndFeel_V4
{
public:
    WavetableLookAndFeel();

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPos, float startAngle, float endAngle,
                          juce::Slider& slider) override;

    void drawButtonBackground(juce::Graphics& g, juce::Button& button,
                              const juce::Colour& backgroundColour,
                              bool shouldDrawButtonAsHighlighted,
                              bool shouldDrawButtonAsDown) override;

    void drawLabel(juce::Graphics& g, juce::Label& label) override;

    juce::Colour bgDark;
    juce::Colour bgPanel;
    juce::Colour cyan;
    juce::Colour textColor;
    juce::Colour gridColor;
    juce::Colour knobTrack;
};
