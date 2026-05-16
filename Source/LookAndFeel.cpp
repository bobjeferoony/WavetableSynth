#include "LookAndFeel.h"

WavetableLookAndFeel::WavetableLookAndFeel()
    : bgDark(0xFF1A1A2E),
      bgPanel(0xFF16213E),
      cyan(0xFF00D4FF),
      textColor(0xFFE0E0E0),
      gridColor(0xFF2A2A4E),
      knobTrack(0xFF2A2A4E)
{
    setColour(juce::Slider::rotarySliderFillColourId, cyan);
    setColour(juce::Slider::rotarySliderOutlineColourId, knobTrack);
    setColour(juce::Slider::textBoxTextColourId, textColor);
    setColour(juce::Slider::textBoxBackgroundColourId, bgDark);
    setColour(juce::Slider::textBoxOutlineColourId, bgDark);

    setColour(juce::Label::textColourId, textColor);
    setColour(juce::TextButton::buttonColourId, cyan);
    setColour(juce::ComboBox::backgroundColourId, bgPanel);
    setColour(juce::ComboBox::textColourId, textColor);
    setColour(juce::ComboBox::outlineColourId, cyan);
    setColour(juce::ComboBox::arrowColourId, cyan);
}

void WavetableLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y,
                                             int width, int height,
                                             float sliderPos,
                                             float startAngle, float endAngle,
                                             juce::Slider& slider)
{
    auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(4);

    g.setColour(knobTrack);
    g.fillEllipse(bounds);

    auto angleRange = endAngle - startAngle;
    float angle = startAngle + sliderPos * angleRange;

    juce::Path arc;
    arc.addArc(bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight(),
               startAngle, angle, true);

    g.setColour(cyan);
    g.strokePath(arc, juce::PathStrokeType(3.0f));

    juce::Line<float> line(
        bounds.getCentreX(),
        bounds.getCentreY(),
        bounds.getCentreX() + std::cos(angle - juce::MathConstants<float>::pi / 2) * bounds.getWidth() * 0.4f,
        bounds.getCentreY() + std::sin(angle - juce::MathConstants<float>::pi / 2) * bounds.getHeight() * 0.4f
    );

    g.setColour(textColor);
    g.drawLine(line, 2.0f);

    g.setColour(cyan.withAlpha(0.3f));
    g.drawEllipse(bounds, 1.0f);
}

void WavetableLookAndFeel::drawButtonBackground(juce::Graphics& g,
                                                 juce::Button& button,
                                                 const juce::Colour& /*bgColor*/,
                                                 bool highlighted, bool down)
{
    auto bounds = button.getLocalBounds().toFloat();
    auto colour = cyan;

    if (down)
        colour = colour.brighter(0.5f);
    else if (highlighted)
        colour = colour.withAlpha(0.8f);
    else
        colour = colour.withAlpha(0.6f);

    g.setColour(bgPanel);
    g.fillRoundedRectangle(bounds, 3.0f);

    g.setColour(colour);
    g.drawRoundedRectangle(bounds, 3.0f, 1.0f);

    if (down || highlighted)
    {
        g.setColour(colour.withAlpha(0.2f));
        g.fillRoundedRectangle(bounds, 3.0f);
    }
}

void WavetableLookAndFeel::drawLabel(juce::Graphics& g, juce::Label& label)
{
    g.setColour(textColor);
    g.setFont(12.0f);

    auto bounds = label.getLocalBounds().toFloat();
    g.drawFittedText(label.getText(), bounds.toNearestInt(),
                     juce::Justification::centred, 1);
}
