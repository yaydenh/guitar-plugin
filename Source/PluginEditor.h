/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "FrequencyVisualiser.h"
#include "AmpKnob.h"

typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
typedef juce::AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;
typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

//==============================================================================
/**
*/
class GuitarAmpAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    GuitarAmpAudioProcessorEditor (GuitarAmpAudioProcessor&, juce::AudioProcessorValueTreeState& vts);
    ~GuitarAmpAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void createKnob(juce::Slider& knob, juce::Label& label, double min, double max, double interval, std::string text);

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    GuitarAmpAudioProcessor& audioProcessor;

    juce::TabbedComponent tabs;

    juce::ToggleButton testToggle;
    std::unique_ptr<ButtonAttachment> testToggleAttachment;
    juce::Label testLabel;

    FrequencyVisualiser preFreq;
    FrequencyVisualiser postFreq;
    juce::AudioVisualiserComponent preVisualiser = juce::AudioVisualiserComponent(1);
    juce::AudioVisualiserComponent postVisualiser = juce::AudioVisualiserComponent(1);

    juce::AudioProcessorValueTreeState& valueTreeState;

    juce::ComboBox preampModeSelector;
    std::unique_ptr<ComboBoxAttachment> preampModeAttachment;
    juce::Label preampModeLabel;

    AmpKnob preGain;

    AmpKnob preBass;
    AmpKnob preMid;
    AmpKnob preTreble;

    juce::ComboBox distortionTypeSelector;
    std::unique_ptr<ComboBoxAttachment> distortionTypeAttachment;
    AmpKnob drive;

    AmpKnob postBass;
    AmpKnob postMid;
    AmpKnob postTreble;
    
    AmpKnob postGain;

    AmpKnob noiseGate;

    juce::ComboBox irSelector;
    juce::Label irLabel;
    std::unique_ptr<ComboBoxAttachment> irAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuitarAmpAudioProcessorEditor)
};
