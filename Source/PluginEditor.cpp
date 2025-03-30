/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GuitarAmpAudioProcessorEditor::GuitarAmpAudioProcessorEditor(GuitarAmpAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor(&p), audioProcessor(p), valueTreeState(vts)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (800, 1000);

    // toggle button for testing
    addAndMakeVisible(testToggle);
    testToggleAttachment.reset(new ButtonAttachment(valueTreeState, "test", testToggle));
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
    preampModeAttachment.reset(new ComboBoxAttachment(valueTreeState, "preampMode", preampModeSelector));
    preampModeLabel.setText("Mode", juce::NotificationType::dontSendNotification);
    preampModeLabel.attachToComponent(&preampModeSelector, false);

    // pre gain knob
    createKnob(preGainSlider, preGainLabel, -24.0f, 24.0f, 0.1f, "Input");
    preGainAttachment.reset(new SliderAttachment(valueTreeState, "preGain", preGainSlider));

    // pre eq knobs
    createKnob(preBassSlider, preBassLabel, -12.0f, 12.0f, 0.1f, "bass");
    preBassAttachment.reset(new SliderAttachment(valueTreeState, "preBassEQ", preBassSlider));
    createKnob(preMidSlider, preMidLabel, -12.0f, 12.0f, 0.1f, "mid");
    preMidAttachment.reset(new SliderAttachment(valueTreeState, "preMidEQ", preMidSlider));
    createKnob(preTrebleSlider, preTrebleLabel, -12.0f, 12.0f, 0.1f, "treble");
    preTrebleAttachment.reset(new SliderAttachment(valueTreeState, "preTrebleEQ", preTrebleSlider));

    // distortion drive knob
    createKnob(driveSlider, driveLabel, 1.0f, 10.0f, 0.1f, "Drive");
    driveAttachment.reset(new SliderAttachment(valueTreeState, "drive", driveSlider));

    // distortion type selector
    distortionTypeSelector.addItem("Soft Clip", 1);
    distortionTypeSelector.addItem("Hard Clip", 2);
    distortionTypeSelector.addItem("WaveShaper", 3);
    distortionTypeSelector.addItem("Fuzz", 4);
    distortionTypeSelector.addItem("None", 5);
    addAndMakeVisible(distortionTypeSelector);
    distortionTypeAttachment.reset(new ComboBoxAttachment(valueTreeState, "distortionType", distortionTypeSelector));

    // post eq knobs
    createKnob(postBassSlider, postBassLabel, -12.0f, 12.0f, 0.1f, "bass");
    postBassAttachment.reset(new SliderAttachment(valueTreeState, "postBassEQ", postBassSlider));
    createKnob(postMidSlider, postMidLabel, -12.0f, 12.0f, 0.1f, "mid");
    postMidAttachment.reset(new SliderAttachment(valueTreeState, "postMidEQ", postMidSlider));
    createKnob(postTrebleSlider, postTrebleLabel, -12.0f, 12.0f, 0.1f, "treble");
    postTrebleAttachment.reset(new SliderAttachment(valueTreeState, "postTrebleEQ", postTrebleSlider));

    // post gain knob
    createKnob(postGainSlider, postGainLabel, -24.0f, 24.0f, 0.1f, "Output");
    postGainAttachment.reset(new SliderAttachment(valueTreeState, "postGain", postGainSlider));

    // noise gate threshold knob
    createKnob(noiseGateThresholdSlider, noiseGateLabel, -96.0f, 0.0f, 0.1f, "Noise Gate");
    noiseGateThresholdAttachment.reset(new SliderAttachment(valueTreeState, "noiseGateThreshold", noiseGateThresholdSlider));

    // cab sim ir selector
    irSelector.addItemList(audioProcessor.getImpulseResponseFilenames(), 1);
    addAndMakeVisible(irSelector);
    irAttachment.reset(new ComboBoxAttachment(valueTreeState, "irChoice", irSelector));
    irLabel.setText("IR", juce::NotificationType::dontSendNotification);
    irLabel.attachToComponent(&irSelector, false);
    irSelector.onChange = [this] { audioProcessor.setImpulseResponse(audioProcessor.getImpulseResponseFilenames()[irSelector.getSelectedId() - 1]); };
}

GuitarAmpAudioProcessorEditor::~GuitarAmpAudioProcessorEditor()
{
}

//==============================================================================
void GuitarAmpAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
}

void GuitarAmpAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    // test button
    testToggle.setBounds(200, 20, 30, 30);

    auto marginLeft = 20;
    auto marginTop = 20;

    //left controls
    preampModeSelector.setBounds(marginLeft, marginTop, 100, 30);

    irSelector.setBounds(marginLeft + 300, marginTop, 100, 30);

    auto preX = marginLeft + 60;
    auto preY = marginTop + 60;
    preBassSlider.setBounds(preX, preY, 80, 80);
    preMidSlider.setBounds(preX + 50, preY, 80, 80);
    preTrebleSlider.setBounds(preX + 100, preY, 80, 80);
    preGainSlider.setBounds(preX - 80, preY + 40, 100, 100);
    
    auto postX = preX;
    auto postY = preY + 100;
    postBassSlider.setBounds(postX, postY, 80, 80);
    postMidSlider.setBounds(postX + 50, postY, 80, 80);
    postTrebleSlider.setBounds(postX + 100, postY, 80, 80);
    postGainSlider.setBounds(postX + 160, preY + 40, 100, 100);

    // right controls
    driveSlider.setBounds(marginLeft + 330, preY + 40, 100, 100);
    distortionTypeSelector.setBounds(marginLeft + 330, preY + 140, 100, 30);
    noiseGateThresholdSlider.setBounds(marginLeft + 330 + 100, preY + 40, 100, 100);

    // visualisers
    preFreq.setBounds(0, 300, 400, 300);
    postFreq.setBounds(0, 600, 400, 300);
    preVisualiser.setBounds(400, 300, 400, 300);
    postVisualiser.setBounds(400, 600, 400, 300);

}

void GuitarAmpAudioProcessorEditor::createKnob(juce::Slider& knob, juce::Label& label, double min, double max, double interval, std::string text)
{
    knob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    knob.setRange(min, max, interval);
    knob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    label.setText(text, juce::NotificationType::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.attachToComponent(&knob, false);
    addAndMakeVisible(knob);
}

