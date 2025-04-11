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
      drive(1.0f, 10.0f, 0.1f, "Drive", "drive", vts),
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

    addAndMakeVisible(preVisualiser);
    audioProcessor.setPreWaveformVisualiser(&preVisualiser);
    addAndMakeVisible(postVisualiser);
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

    // distortion drive knob
    addAndMakeVisible(drive);

    // distortion type selector
    distortionTypeSelector.addItem("Soft Clip", 1);
    distortionTypeSelector.addItem("Hard Clip", 2);
    distortionTypeSelector.addItem("WaveShaper", 3);
    distortionTypeSelector.addItem("Fuzz", 4);
    distortionTypeSelector.addItem("None", 5);
    addAndMakeVisible(distortionTypeSelector);
    distortionTypeAttachment.reset(new ComboBoxAttachment(vts, "distortionType", distortionTypeSelector));

    // post eq knobs
    addAndMakeVisible(postBass);
    addAndMakeVisible(postMid);
    addAndMakeVisible(postTreble);

    // post gain knob
    addAndMakeVisible(postGain);

    // noise gate threshold knob
    addAndMakeVisible(noiseGate);

    // cab sim ir selector
    irSelector.addItem("None", 1);
    irSelector.setSelectedId(1);
    irSelector.addItemList(audioProcessor.getImpulseResponseFilenames(), 2);
    addAndMakeVisible(irSelector);
    irLabel.setText("IR", juce::NotificationType::dontSendNotification);
    irLabel.attachToComponent(&irSelector, false);
    irSelector.onChange = [this] { audioProcessor.setImpulseResponse(audioProcessor.getImpulseResponseFilenames()[irSelector.getSelectedId() - 1]); };
}

void TabMain::resized()
{
    // test button
    testToggle.setBounds(200, 20, 30, 30);

    auto marginLeft = 20;
    auto marginTop = 20;

    //left controls
    preampModeSelector.setBounds(marginLeft, marginTop, 100, 30);

    irSelector.setBounds(marginLeft + 300, marginTop, 100, 30);

    auto preX = marginLeft + 60;
    auto preY = marginTop + 60;
    toneStackBass.setBounds(preX, preY, 80, 80);
    toneStackMid.setBounds(preX + 50, preY, 80, 80);
    toneStackTreble.setBounds(preX + 100, preY, 80, 80);
    preGain.setBounds(preX - 80, preY + 40, 100, 100);

    auto postX = preX;
    auto postY = preY + 100;
    postBass.setBounds(postX, postY, 80, 80);
    postMid.setBounds(postX + 50, postY, 80, 80);
    postTreble.setBounds(postX + 100, postY, 80, 80);
    postGain.setBounds(postX + 160, preY + 40, 100, 100);

    // right controls
    drive.setBounds(marginLeft + 330, preY + 40, 100, 100);
    distortionTypeSelector.setBounds(marginLeft + 330, preY + 140, 100, 30);
    noiseGate.setBounds(marginLeft + 330 + 100, preY + 40, 100, 100);

    // visualisers
    preFreq.setBounds(0, 300, 400, 300);
    postFreq.setBounds(0, 600, 400, 300);
    preVisualiser.setBounds(400, 300, 400, 300);
    postVisualiser.setBounds(400, 600, 400, 300);
}