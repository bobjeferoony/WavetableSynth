#include "PluginEditor.h"

// ===== WavetableDisplay =====
WavetableDisplay::WavetableDisplay(WavetableSynthAudioProcessor& p)
    : processor(p)
{
    currentWave.resize(512, 0.0f);
}

void WavetableDisplay::setWavetableData(const std::vector<float>* data)
{
    if (data && !data->empty())
    {
        int step = juce::jmax(1, (int)data->size() / (int)currentWave.size());
        for (size_t i = 0; i < currentWave.size(); ++i)
            currentWave[i] = (*data)[(i * step) % data->size()];
    }
    repaint();
}

void WavetableDisplay::updateFromPosition(float position)
{
    const auto& tables = processor.wavetableBank;
    if (tables.empty()) return;

    float tableIdx = position * (tables.size() - 1);
    int idxA = (int)tableIdx;
    int idxB = juce::jmin(idxA + 1, (int)tables.size() - 1);
    float frac = tableIdx - idxA;

    for (size_t i = 0; i < currentWave.size(); ++i)
    {
        float p = (float)i / currentWave.size();
        float pos = p * (tables[0].size() - 1);
        int sampIdx = (int)pos;
        sampIdx = juce::jmin(sampIdx, (int)tables[0].size() - 1);
        float v = tables[idxA][sampIdx] * (1.0f - frac) + tables[idxB][sampIdx] * frac;
        currentWave[i] = v;
    }
    repaint();
}

void WavetableDisplay::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(4);
    g.setColour(lnf.bgPanel);
    g.fillRoundedRectangle(bounds, 4);

    g.setColour(lnf.gridColor);
    for (int i = 0; i < 4; ++i)
    {
        float y = bounds.getY() + bounds.getHeight() * (i + 1) / 5.0f;
        g.drawHorizontalLine((int)y, bounds.getX(), bounds.getRight());
    }
    for (int i = 0; i < 8; ++i)
    {
        float x = bounds.getX() + bounds.getWidth() * (i + 1) / 9.0f;
        g.drawVerticalLine((int)x, bounds.getY(), bounds.getBottom());
    }

    if (currentWave.size() < 2) return;
    float midY = bounds.getCentreY();
    float halfH = bounds.getHeight() * 0.45f;

    juce::Path wavePath;
    wavePath.startNewSubPath(bounds.getX(), midY - currentWave[0] * halfH);
    for (size_t i = 1; i < currentWave.size(); ++i)
    {
        float x = bounds.getX() + bounds.getWidth() * ((float)i / currentWave.size());
        float y = midY - currentWave[i] * halfH;
        wavePath.lineTo(x, y);
    }
    g.setColour(lnf.cyan);
    g.strokePath(wavePath, juce::PathStrokeType(2.0f));
}

// ===== AdsrEditor =====
AdsrEditor::AdsrEditor(WavetableSynthAudioProcessor& p,
                       juce::AudioProcessorValueTreeState& a)
    : processor(p), apvts(a)
{
}

void AdsrEditor::resized() {}

void AdsrEditor::updateFromParams()
{
    attack  = apvts.getRawParameterValue("attack")->load();
    decay   = apvts.getRawParameterValue("decay")->load();
    sustain = apvts.getRawParameterValue("sustain")->load();
    release = apvts.getRawParameterValue("release")->load();
    repaint();
}

juce::Rectangle<float> AdsrEditor::getAttackRect() const
{
    auto b = getLocalBounds().toFloat().reduced(4);
    float total = attack + decay + release;
    total = juce::jmax(0.1f, total);
    float x = b.getX() + (attack / total) * b.getWidth() * 0.7f;
    return { x - 4, b.getY() + 10, 8, 20 };
}

juce::Rectangle<float> AdsrEditor::getDecayRect() const
{
    auto b = getLocalBounds().toFloat().reduced(4);
    float total = attack + decay + release;
    total = juce::jmax(0.1f, total);
    float x = b.getX() + ((attack + decay) / total) * b.getWidth() * 0.7f;
    float sustainY = b.getBottom() - 10 - (b.getHeight() - 20) * sustain;
    return { x - 4, sustainY - 10, 8, 20 };
}

juce::Rectangle<float> AdsrEditor::getSustainRect() const
{
    auto b = getLocalBounds().toFloat().reduced(4);
    float x = b.getX() + b.getWidth() * 0.72f;
    float sustainY = b.getBottom() - 10 - (b.getHeight() - 20) * sustain;
    return { x - 4, sustainY - 4, 8, 8 };
}

juce::Rectangle<float> AdsrEditor::getReleaseRect() const
{
    auto b = getLocalBounds().toFloat().reduced(4);
    float total = attack + decay + release;
    total = juce::jmax(0.1f, total);
    float x = b.getX() + (1.0f - release / total * 0.3f) * b.getWidth();
    return { x - 4, b.getY() + 10, 8, 20 };
}

void AdsrEditor::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(4);
    g.setColour(lnf.bgPanel);
    g.fillRoundedRectangle(bounds, 4);

    g.setColour(lnf.gridColor);
    for (int i = 0; i < 4; ++i)
    {
        float y = bounds.getY() + bounds.getHeight() * (i + 1) / 5.0f;
        g.drawHorizontalLine((int)y, bounds.getX(), bounds.getRight());
    }

    float w = bounds.getWidth();
    float h = bounds.getHeight();
    float total = juce::jmax(0.1f, attack + decay + release);
    float attEnd = (attack / total) * w * 0.7f;
    float decEnd = attEnd + (decay / total) * w * 0.7f;
    float susY = h - 10 - (h - 20) * sustain;
    float relStart = w * 0.72f;
    float relEnd = w;

    juce::Path envPath;
    envPath.startNewSubPath(bounds.getX(), bounds.getBottom() - 10);
    envPath.lineTo(bounds.getX() + attEnd, bounds.getY() + 10);
    envPath.lineTo(bounds.getX() + decEnd, bounds.getY() + susY);
    envPath.lineTo(bounds.getX() + relStart, bounds.getY() + susY);
    envPath.lineTo(bounds.getX() + relEnd, bounds.getBottom() - 10);

    g.setColour(lnf.cyan.withAlpha(0.3f));
    g.strokePath(envPath, juce::PathStrokeType(3.0f));
    g.setColour(lnf.cyan);
    g.strokePath(envPath, juce::PathStrokeType(1.5f));

    auto drawHandle = [&](juce::Rectangle<float> r)
    {
        g.setColour(lnf.cyan);
        g.fillRoundedRectangle(r, 3);
    };

    drawHandle(getAttackRect());
    drawHandle(getDecayRect());
    drawHandle(getSustainRect());
    drawHandle(getReleaseRect());
}

void AdsrEditor::mouseDown(const juce::MouseEvent& e)
{
    auto pos = e.position;
    if (getAttackRect().contains(pos))  dragHandle = AttackH;
    else if (getDecayRect().contains(pos)) dragHandle = DecayH;
    else if (getSustainRect().contains(pos)) dragHandle = SustainH;
    else if (getReleaseRect().contains(pos)) dragHandle = ReleaseH;
    else dragHandle = None;
}

void AdsrEditor::mouseDrag(const juce::MouseEvent& e)
{
    auto bounds = getLocalBounds().toFloat().reduced(4);
    float relX = (e.position.x - bounds.getX()) / bounds.getWidth();
    float relY = 1.0f - (e.position.y - bounds.getY()) / bounds.getHeight();
    relX = juce::jlimit(0.0f, 1.0f, relX);
    relY = juce::jlimit(0.0f, 1.0f, relY);

    switch (dragHandle)
    {
        case AttackH:
            attack = relX * 5.0f + 0.001f;
            *apvts.getRawParameterValue("attack") = attack;
            break;
        case DecayH:
            decay = relX * 5.0f + 0.001f;
            *apvts.getRawParameterValue("decay") = decay;
            break;
        case SustainH:
            sustain = relY;
            *apvts.getRawParameterValue("sustain") = sustain;
            break;
        case ReleaseH:
            release = (1.0f - relX) * 5.0f + 0.001f;
            *apvts.getRawParameterValue("release") = release;
            break;
        default:
            break;
    }
    updateFromParams();
}

// ===== LfoDisplay =====
LfoDisplay::LfoDisplay(WavetableSynthAudioProcessor& p, int idx)
    : processor(p), lfoIndex(idx)
{
}

void LfoDisplay::updateFromParams()
{
    juce::String rateId  = (lfoIndex == 0) ? "lfo1_rate" : "lfo2_rate";
    juce::String shapeId = (lfoIndex == 0) ? "lfo1_shape" : "lfo2_shape";
    rate  = processor.apvts.getRawParameterValue(rateId)->load();
    shape = (int)processor.apvts.getRawParameterValue(shapeId)->load();
    repaint();
}

void LfoDisplay::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(4);
    g.setColour(lnf.bgPanel);
    g.fillRoundedRectangle(bounds, 4);

    g.setColour(lnf.gridColor);
    for (int i = 0; i < 3; ++i)
    {
        float y = bounds.getY() + bounds.getHeight() * (i + 1) / 4.0f;
        g.drawHorizontalLine((int)y, bounds.getX(), bounds.getRight());
    }

    int numPoints = (int)bounds.getWidth() / 2;
    float midY = bounds.getCentreY();
    float halfH = bounds.getHeight() * 0.4f;

    juce::Path wavePath;
    bool first = true;
    for (int i = 0; i < numPoints; ++i)
    {
        float p = (float)i / numPoints;
        float val = 0.0f;
        switch (shape)
        {
            case 0: val = std::sin(2.0f * juce::MathConstants<float>::pi * p); break;
            case 1: val = 2.0f * p - 1.0f; break;
            case 2: val = (p < 0.5f) ? 1.0f : -1.0f; break;
            case 3: val = 4.0f * std::abs(p - 0.5f) - 1.0f; break;
            case 4: val = ((int)(p * 8) % 2 == 0) ? 1.0f : -1.0f; break;
            default: val = 0.0f;
        }
        float x = bounds.getX() + bounds.getWidth() * ((float)i / numPoints);
        float y = midY - val * halfH;
        if (first) { wavePath.startNewSubPath(x, y); first = false; }
        else wavePath.lineTo(x, y);
    }

    g.setColour(lnf.cyan);
    g.strokePath(wavePath, juce::PathStrokeType(1.5f));
}

// ===== Plugin Editor =====
WavetableSynthAudioProcessorEditor::WavetableSynthAudioProcessorEditor(
    WavetableSynthAudioProcessor& p)
    : AudioProcessorEditor(p),
      processor(p),
      wtDisplay(p),
      adsrEditor(p, p.apvts),
      lfo1Display(p, 0),
      lfo2Display(p, 1),
      midiKeyboard(p.midiKeyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
{
    setLookAndFeel(&lnf);
    setSize(1000, 650);
    setResizable(true, true);
    setResizeLimits(800, 520, 1600, 1040);

    titleLabel.setText("WavetableSynth", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(20.0f, juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(titleLabel);

    initButtonStyle(initButton, "Init");
    addAndMakeVisible(initButton);

    // Oscillator section
    initSlider(wtPositionSlider, "WT Pos");
    initSlider(unisonSlider, "Unison");
    initSlider(detuneSlider, "Detune");
    initSlider(blendSlider, "Blend");
    addAndMakeVisible(wtDisplay);

    // Filter section
    initSlider(filterCutoffSlider, "Cutoff");
    initSlider(filterResSlider, "Resonance");
    initCombo(filterTypeBox, { "Low-Pass", "High-Pass", "Band-Pass" });

    // LFO sections
    initSlider(lfo1RateSlider, "Rate");
    initCombo(lfo1ShapeBox, { "Sine", "Saw", "Square", "Triangle", "S&H" });
    initSlider(lfo1WtAmtSlider, "->WT");
    initSlider(lfo1FilAmtSlider, "->Fil");
    initSlider(lfo1GainAmtSlider, "->Gain");

    initSlider(lfo2RateSlider, "Rate");
    initCombo(lfo2ShapeBox, { "Sine", "Saw", "Square", "Triangle", "S&H" });
    initSlider(lfo2WtAmtSlider, "->WT");
    initSlider(lfo2FilAmtSlider, "->Fil");
    initSlider(lfo2GainAmtSlider, "->Gain");

    initSlider(envFilAmtSlider, "Env->Fil");

    // Macros
    initSlider(macroSlider1, "Macro 1");
    initSlider(macroSlider2, "Macro 2");
    initSlider(macroSlider3, "Macro 3");
    initSlider(macroSlider4, "Macro 4");

    // Gain
    initSlider(gainSlider, "Gain");

    // ADSR and LFO displays
    addAndMakeVisible(adsrEditor);
    addAndMakeVisible(lfo1Display);
    addAndMakeVisible(lfo2Display);

    // MIDI keyboard
    midiKeyboard.setAvailableRange(24, 108);
    addAndMakeVisible(midiKeyboard);

    // Create attachments
    wtPosAtt       = std::make_unique<SliderAtt>(p.apvts, "wt_position", wtPositionSlider);
    unisonAtt      = std::make_unique<SliderAtt>(p.apvts, "unison", unisonSlider);
    detuneAtt      = std::make_unique<SliderAtt>(p.apvts, "detune", detuneSlider);
    blendAtt       = std::make_unique<SliderAtt>(p.apvts, "wt_blend", blendSlider);
    filterCutoffAtt= std::make_unique<SliderAtt>(p.apvts, "filter_cutoff", filterCutoffSlider);
    filterResAtt   = std::make_unique<SliderAtt>(p.apvts, "filter_res", filterResSlider);
    filterTypeAtt  = std::make_unique<ComboAtt>(p.apvts, "filter_type", filterTypeBox);
    lfo1RateAtt    = std::make_unique<SliderAtt>(p.apvts, "lfo1_rate", lfo1RateSlider);
    lfo1ShapeAtt   = std::make_unique<ComboAtt>(p.apvts, "lfo1_shape", lfo1ShapeBox);
    lfo1WtAmtAtt   = std::make_unique<SliderAtt>(p.apvts, "lfo1_wt_amt", lfo1WtAmtSlider);
    lfo1FilAmtAtt  = std::make_unique<SliderAtt>(p.apvts, "lfo1_fil_amt", lfo1FilAmtSlider);
    lfo1GainAmtAtt = std::make_unique<SliderAtt>(p.apvts, "lfo1_gain_amt", lfo1GainAmtSlider);
    lfo2RateAtt    = std::make_unique<SliderAtt>(p.apvts, "lfo2_rate", lfo2RateSlider);
    lfo2ShapeAtt   = std::make_unique<ComboAtt>(p.apvts, "lfo2_shape", lfo2ShapeBox);
    lfo2WtAmtAtt   = std::make_unique<SliderAtt>(p.apvts, "lfo2_wt_amt", lfo2WtAmtSlider);
    lfo2FilAmtAtt  = std::make_unique<SliderAtt>(p.apvts, "lfo2_fil_amt", lfo2FilAmtSlider);
    lfo2GainAmtAtt = std::make_unique<SliderAtt>(p.apvts, "lfo2_gain_amt", lfo2GainAmtSlider);
    envFilAmtAtt   = std::make_unique<SliderAtt>(p.apvts, "env_fil_amt", envFilAmtSlider);
    macroAtt1      = std::make_unique<SliderAtt>(p.apvts, "macro1", macroSlider1);
    macroAtt2      = std::make_unique<SliderAtt>(p.apvts, "macro2", macroSlider2);
    macroAtt3      = std::make_unique<SliderAtt>(p.apvts, "macro3", macroSlider3);
    macroAtt4      = std::make_unique<SliderAtt>(p.apvts, "macro4", macroSlider4);
    gainAtt        = std::make_unique<SliderAtt>(p.apvts, "gain", gainSlider);

    wtPositionSlider.onValueChange = [this] {
        wtDisplay.updateFromPosition((float)wtPositionSlider.getValue());
    };

    initButton.onClick = [this] {
        processor.apvts.state = juce::ValueTree("Parameters");
        wtDisplay.updateFromPosition(0.0f);
        adsrEditor.updateFromParams();
        lfo1Display.updateFromParams();
        lfo2Display.updateFromParams();
    };

    startTimerHz(30);
    wtDisplay.updateFromPosition(0.0f);
    adsrEditor.updateFromParams();
    lfo1Display.updateFromParams();
    lfo2Display.updateFromParams();
}

WavetableSynthAudioProcessorEditor::~WavetableSynthAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

void WavetableSynthAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.setColour(lnf.bgDark);
    g.fillAll();

    auto bounds = getLocalBounds().toFloat();
    g.setColour(lnf.gridColor.withAlpha(0.2f));
    for (int x = 0; x < getWidth(); x += 40)
        g.drawVerticalLine(x, 0.0f, (float)getHeight());
    for (int y = 0; y < getHeight(); y += 40)
        g.drawHorizontalLine(y, 0.0f, (float)getWidth());
}

void WavetableSynthAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();
    int topBarH = 36;
    int keyboardH = 56;
    int margin = 5;
    int sw = 52;
    int sh = 58;

    auto topBar = area.removeFromTop(topBarH).reduced(margin);
    titleLabel.setBounds(topBar.removeFromLeft(200));
    initButton.setBounds(topBar.removeFromRight(60).withSizeKeepingCentre(50, 24));

    auto bottomKeyboard = area.removeFromBottom(keyboardH);
    midiKeyboard.setBounds(bottomKeyboard.reduced(margin));

    int panelW = (area.getWidth() - margin * 4) / 3;

    // ========== Left Panel: Oscillator ==========
    auto leftP = area.removeFromLeft(panelW).reduced(margin);
    {
        auto wtArea = leftP.removeFromTop(juce::jmin(140, leftP.getHeight() / 2));
        wtDisplay.setBounds(wtArea.reduced(2));

        auto ctrlArea = leftP.reduced(4);
        int rowH = sh + 16;
        auto r1 = ctrlArea.removeFromTop(rowH);
        int gap = (r1.getWidth() - sw * 4) / 3;
        r1.removeFromLeft(gap / 2);
        wtPositionSlider.setBounds(r1.removeFromLeft(sw));
        r1.removeFromLeft(gap);
        unisonSlider.setBounds(r1.removeFromLeft(sw));
        r1.removeFromLeft(gap);
        detuneSlider.setBounds(r1.removeFromLeft(sw));
        r1.removeFromLeft(gap);
        blendSlider.setBounds(r1.removeFromLeft(sw));
    }

    // ========== Middle Panel: ADSR + LFOs ==========
    auto midP = area.removeFromLeft(panelW).reduced(margin);
    {
        auto envArea = midP.removeFromTop(midP.getHeight() / 2);
        adsrEditor.setBounds(envArea.reduced(2));

        auto lfoP = midP.reduced(2);
        auto lfoH = lfoP.getHeight() / 2;

        // LFO1
        auto l1Area = lfoP.removeFromTop(lfoH);
        auto l1Disp = l1Area.removeFromLeft(l1Area.getWidth() / 2);
        lfo1Display.setBounds(l1Disp.reduced(1));
        auto l1C = l1Area.reduced(2);
        {
            int cw = sw;
            int cgap = (l1C.getWidth() - cw * 4) / 3;
            auto shapeRow = l1C.removeFromTop(18);
            lfo1ShapeBox.setBounds(shapeRow.withSizeKeepingCentre(100, 16));
            auto sliderRow = l1C.reduced(0, 2);
            int sgap = (sliderRow.getWidth() - cw * 4) / 3;
            sliderRow.removeFromLeft(sgap / 2);
            lfo1RateSlider.setBounds(sliderRow.removeFromLeft(cw));
            sliderRow.removeFromLeft(sgap);
            lfo1WtAmtSlider.setBounds(sliderRow.removeFromLeft(cw));
            sliderRow.removeFromLeft(sgap);
            lfo1FilAmtSlider.setBounds(sliderRow.removeFromLeft(cw));
            sliderRow.removeFromLeft(sgap);
            lfo1GainAmtSlider.setBounds(sliderRow.removeFromLeft(cw));
        }

        // LFO2
        auto l2Area = lfoP;
        auto l2Disp = l2Area.removeFromLeft(l2Area.getWidth() / 2);
        lfo2Display.setBounds(l2Disp.reduced(1));
        auto l2C = l2Area.reduced(2);
        {
            int cw = sw;
            int cgap = (l2C.getWidth() - cw * 4) / 3;
            auto shapeRow = l2C.removeFromTop(18);
            lfo2ShapeBox.setBounds(shapeRow.withSizeKeepingCentre(100, 16));
            auto sliderRow = l2C.reduced(0, 2);
            int sgap = (sliderRow.getWidth() - cw * 4) / 3;
            sliderRow.removeFromLeft(sgap / 2);
            lfo2RateSlider.setBounds(sliderRow.removeFromLeft(cw));
            sliderRow.removeFromLeft(sgap);
            lfo2WtAmtSlider.setBounds(sliderRow.removeFromLeft(cw));
            sliderRow.removeFromLeft(sgap);
            lfo2FilAmtSlider.setBounds(sliderRow.removeFromLeft(cw));
            sliderRow.removeFromLeft(sgap);
            lfo2GainAmtSlider.setBounds(sliderRow.removeFromLeft(cw));
        }
    }

    // ========== Right Panel: Filter + Macros + Gain ==========
    auto rightP = area.reduced(margin);
    {
        // Filter section
        auto filtArea = rightP.removeFromTop(juce::jmin(110, rightP.getHeight() / 3));
        auto filtC = filtArea.reduced(4);
        {
            int cw = sw;
            filterCutoffSlider.setBounds(filtC.removeFromLeft(cw));
            filtC.removeFromLeft(6);
            filterResSlider.setBounds(filtC.removeFromLeft(cw));
            filtC.removeFromLeft(6);
            filterTypeBox.setBounds(filtC.removeFromLeft(90).withHeight(20));
            filtC.removeFromLeft(6);
            envFilAmtSlider.setBounds(filtC.removeFromLeft(cw));
        }

        // Macros
        auto macArea = rightP.reduced(4);

        auto macRows = macArea.removeFromTop(macArea.getHeight() / 2);
        int mgap = (macRows.getWidth() - sw * 2) / 3;
        macroSlider1.setBounds(macRows.removeFromLeft(sw));
        macRows.removeFromLeft(mgap);
        macroSlider2.setBounds(macRows.removeFromLeft(sw));

        auto macRow2 = macArea;
        macroSlider3.setBounds(macRow2.removeFromLeft(sw));
        macRow2.removeFromLeft(mgap);
        macroSlider4.setBounds(macRow2.removeFromLeft(sw));

        // Gain
        auto gArea = rightP.removeFromBottom(sh + 16);
        gainSlider.setBounds(gArea.removeFromLeft(sw));
    }
}

void WavetableSynthAudioProcessorEditor::timerCallback()
{
    adsrEditor.updateFromParams();
    lfo1Display.updateFromParams();
    lfo2Display.updateFromParams();
}

void WavetableSynthAudioProcessorEditor::initSlider(juce::Slider& slider,
                                                     const juce::String& label)
{
    slider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 14);
    slider.setColour(juce::Slider::textBoxTextColourId, lnf.textColor);
    slider.setColour(juce::Slider::textBoxBackgroundColourId, lnf.bgDark);
    slider.setColour(juce::Slider::textBoxOutlineColourId, lnf.bgDark);
    slider.setRotaryParameters(juce::MathConstants<float>::pi * 0.75f,
                               juce::MathConstants<float>::pi * 2.25f,
                               true);
    slider.setName(label);
    addAndMakeVisible(slider);
}

void WavetableSynthAudioProcessorEditor::initCombo(juce::ComboBox& box,
                                                    const juce::StringArray& items)
{
    box.addItemList(items, 1);
    box.setSelectedItemIndex(0);
    box.setColour(juce::ComboBox::backgroundColourId, lnf.bgPanel);
    box.setColour(juce::ComboBox::textColourId, lnf.textColor);
    box.setColour(juce::ComboBox::outlineColourId, lnf.cyan);
    box.setColour(juce::ComboBox::arrowColourId, lnf.cyan);
    addAndMakeVisible(box);
}

void WavetableSynthAudioProcessorEditor::initButtonStyle(juce::TextButton& button,
                                                          const juce::String& text)
{
    button.setButtonText(text);
    button.setColour(juce::TextButton::buttonColourId, lnf.bgPanel);
    button.setColour(juce::TextButton::textColourOffId, lnf.cyan);
    button.setColour(juce::TextButton::textColourOnId, lnf.cyan.brighter());
}
