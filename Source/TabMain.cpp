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
    auto marginTop = 30;

    preampModeSelector.setBounds(marginLeft, marginTop, 100, 30);
    irSelector.setBounds(marginLeft + 120, marginTop, 100, 30);
    noiseGate.setBounds(marginLeft + 240, 0, 80, 80);

    auto knobY = marginTop + 60;
    preGain.setBounds(0, knobY, 100, 100);
    toneStackBass.setBounds(100, knobY, 80, 80);
    toneStackMid.setBounds(170, knobY, 80, 80);
    toneStackTreble.setBounds(240, knobY, 80, 80);
    postBass.setBounds(320, knobY, 80, 80);
    postMid.setBounds(390, knobY, 80, 80);
    postTreble.setBounds(460, knobY, 80, 80);
    postGain.setBounds(540, knobY, 100, 100);


    // visualisers
    preFreq.setBounds(0, 200, 320, 200);
    postFreq.setBounds(0, 400, 320, 200);
    preVisualiser.setBounds(320, 200, 320, 200);
    postVisualiser.setBounds(320, 400, 320, 200);
}