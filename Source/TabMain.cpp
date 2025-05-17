/*
  ==============================================================================

    TabMain.cpp
    Created: 1 Apr 2025 3:15:29am
    Author:  jayde

  ==============================================================================
*/

#include "TabMain.h"

TabMain::TabMain(GuitarAmpAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : audioProcessor(p),
      preGain(-24.0f, 24.0f, 0.1f, "Input", "preGain", vts),
      toneStackBass(-12.0f, 12.0f, 0.1f, "bass", "toneStackBass", vts),
      toneStackMid(-12.0f, 12.0f, 0.1f, "mid", "toneStackMid", vts),
      toneStackTreble(-12.0f, 12.0f, 0.1f, "treble", "toneStackTreble", vts),
      postBass(-12.0f, 12.0f, 0.1f, "bass", "postBassEQ", vts),
      postMid(-12.0f, 12.0f, 0.1f, "mid", "postMidEQ", vts),
      postTreble(-12.0f, 12.0f, 0.1f, "treble", "postTrebleEQ", vts),
      postGain(-24.0f, 24.0f, 0.1f, "Output", "postGain", vts),
      noiseGate(-96.0f, 0.0f, 0.1f, "Noise Gate", "noiseGateThreshold", vts)
{
    // toggle button for testing
    addAndMakeVisible(testToggle);
    testToggleAttachment.reset(new ButtonAttachment(vts, "test", testToggle));
    testLabel.setText("Test", juce::NotificationType::dontSendNotification);
    testLabel.attachToComponent(&testToggle, false);

    // visualisers
    addAndMakeVisible(preFreq);
    audioProcessor.setPreFrequencyVisualiser(&preFreq);
    addAndMakeVisible(postFreq);
    audioProcessor.setPostFrequencyVisualiser(&postFreq);

    auto bgColour = getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId);
    addAndMakeVisible(preVisualiser);
    preVisualiser.setColours(bgColour, juce::Colours::black);
    audioProcessor.setPreWaveformVisualiser(&preVisualiser);
    addAndMakeVisible(postVisualiser);
    postVisualiser.setColours(bgColour, juce::Colours::black);
    audioProcessor.setPostWaveformVisualiser(&postVisualiser);

    // distortion type selector
    preampModeSelector.addItem("Clean", 1);
    preampModeSelector.addItem("Crunch", 2);
    preampModeSelector.addItem("Lead", 3);
    addAndMakeVisible(preampModeSelector);
    preampModeAttachment.reset(new ComboBoxAttachment(vts, "preampMode", preampModeSelector));
    preampModeLabel.setText("Mode", juce::NotificationType::dontSendNotification);
    preampModeLabel.attachToComponent(&preampModeSelector, false);

    // pre gain knob
    addAndMakeVisible(preGain);

    // tone stack eq knobs
    addAndMakeVisible(toneStackBass);
    addAndMakeVisible(toneStackMid);
    addAndMakeVisible(toneStackTreble);

    // post eq knobs
    addAndMakeVisible(postBass);
    addAndMakeVisible(postMid);
    addAndMakeVisible(postTreble);

    // post gain knob
    addAndMakeVisible(postGain);

    // noise gate threshold knob
    addAndMakeVisible(noiseGate);

    // cab sim ir selector
    irSelector.addItemList(audioProcessor.getImpulseResponseFilenames(), 1);
    irSelector.setSelectedId(1);
    addAndMakeVisible(irSelector);
    irLabel.setText("IR", juce::NotificationType::dontSendNotification);
    irLabel.attachToComponent(&irSelector, false);
    irSelector.onChange = [this] { audioProcessor.setImpulseResponse(audioProcessor.getImpulseResponseFilenames()[irSelector.getSelectedId() - 1]); };
}

void TabMain::resized()
{
    // test button
    //testToggle.setBounds(200, 20, 30, 30);

    auto marginLeft = 20;
    auto marginTop = 20;

    //left controls
    preampModeSelector.setBounds(marginLeft, marginTop, 100, 30);

    irSelector.setBounds(marginLeft + 120, marginTop, 100, 30);

    auto preX = marginLeft + 60;
    auto preY = marginTop + 60;
    toneStackBass.setBounds(preX, preY, 80, 80);
    toneStackMid.setBounds(preX + 60, preY, 80, 80);
    toneStackTreble.setBounds(preX + 120, preY, 80, 80);
    preGain.setBounds(preX - 80, preY + 40, 100, 100);

    auto postX = preX;
    auto postY = preY + 100;
    postBass.setBounds(postX, postY, 80, 80);
    postMid.setBounds(postX + 60, postY, 80, 80);
    postTreble.setBounds(postX + 120, postY, 80, 80);
    postGain.setBounds(postX + 180, preY + 40, 100, 100);

    // right controls
    noiseGate.setBounds(marginLeft + 330, preY + 40, 100, 100);

    // visualisers
    preFreq.setBounds(0, 300, 400, 300);
    postFreq.setBounds(0, 600, 400, 300);
    preVisualiser.setBounds(400, 300, 400, 300);
    postVisualiser.setBounds(400, 600, 400, 300);
}