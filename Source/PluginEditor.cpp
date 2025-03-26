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

    // pre gain knob
    createKnob(preGainSlider, -24.0f, 24.0f, 0.1f);
    preGainAttachment.reset(new SliderAttachment(valueTreeState, "preGain", preGainSlider));

    // pre eq knobs
    createKnob(preBassSlider, -12.0f, 12.0f, 0.1f);
    preBassAttachment.reset(new SliderAttachment(valueTreeState, "preBassEQ", preBassSlider));
    createKnob(preMidSlider, -12.0f, 12.0f, 0.1f);
    preMidAttachment.reset(new SliderAttachment(valueTreeState, "preMidEQ", preMidSlider));
    createKnob(preTrebleSlider, -12.0f, 12.0f, 0.1f);
    preTrebleAttachment.reset(new SliderAttachment(valueTreeState, "preTrebleEQ", preTrebleSlider));

    // distortion drive knob
    createKnob(driveSlider, 1.0f, 10.0f, 0.1f);
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
    createKnob(postBassSlider, -12.0f, 12.0f, 0.1f);
    postBassAttachment.reset(new SliderAttachment(valueTreeState, "postBassEQ", postBassSlider));
    createKnob(postMidSlider, -12.0f, 12.0f, 0.1f);
    postMidAttachment.reset(new SliderAttachment(valueTreeState, "postMidEQ", postMidSlider));
    createKnob(postTrebleSlider, -12.0f, 12.0f, 0.1f);
    postTrebleAttachment.reset(new SliderAttachment(valueTreeState, "postTrebleEQ", postTrebleSlider));

    // post gain knob
    createKnob(postGainSlider, -24.0f, 24.0f, 0.1f);
    postGainAttachment.reset(new SliderAttachment(valueTreeState, "postGain", postGainSlider));

    // noise gate threshold knob
    createKnob(noiseGateThresholdSlider, -96.0f, 0.0f, 0.1f);
    noiseGateThresholdAttachment.reset(new SliderAttachment(valueTreeState, "noiseGateThreshold", noiseGateThresholdSlider));
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
    testToggle.setBounds(400, 100, 30, 30);

    auto sliderLeft = 100;

    preampModeSelector.setBounds(sliderLeft, 20, 150, 30);

    auto preX = 10;
    auto preY = 50;
    preBassSlider.setBounds(preX, preY, 80, 80);
    preMidSlider.setBounds(preX + 50, preY, 80, 80);
    preTrebleSlider.setBounds(preX + 100, preY, 80, 80);
    preGainSlider.setBounds(preX + 150, preY, 100, 100);

    driveSlider.setBounds(sliderLeft + 150, 20 + 100, 100, 100);
    distortionTypeSelector.setBounds(sliderLeft + 150, 20 + 200, 150, 30);
    
    auto postX = 10;
    auto postY = 150;
    postBassSlider.setBounds(postX, postY, 80, 80);
    postMidSlider.setBounds(postX + 50, postY, 80, 80);
    postTrebleSlider.setBounds(postX + 100, postY, 80, 80);
    postGainSlider.setBounds(postX + 150, postY, 100, 100);

    preFreq.setBounds(0, 300, 400, 300);
    postFreq.setBounds(0, 600, 400, 300);

    preVisualiser.setBounds(400, 300, 400, 300);
    postVisualiser.setBounds(400, 600, 400, 300);

    noiseGateThresholdSlider.setBounds(300, 0, 80, 80);
}

void GuitarAmpAudioProcessorEditor::createKnob(juce::Slider& knob, double min, double max, double interval)
{
    knob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    knob.setRange(-min, max, interval);
    knob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(knob);
}
