/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "FrequencyVisualiser.h"

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

    juce::Slider preGainSlider;
    std::unique_ptr<SliderAttachment> preGainAttachment;
    juce::Label preGainLabel;

    juce::Slider preBassSlider;
    std::unique_ptr<SliderAttachment> preBassAttachment;
    juce::Label preBassLabel;
    juce::Slider preMidSlider;
    std::unique_ptr<SliderAttachment> preMidAttachment;
    juce::Label preMidLabel;
    juce::Slider preTrebleSlider;
    std::unique_ptr<SliderAttachment> preTrebleAttachment;
    juce::Label preTrebleLabel;

    juce::ComboBox distortionTypeSelector;
    std::unique_ptr<ComboBoxAttachment> distortionTypeAttachment;
    juce::Slider driveSlider;
    std::unique_ptr<SliderAttachment> driveAttachment;
    juce::Label driveLabel;

    juce::Slider postBassSlider;
    std::unique_ptr<SliderAttachment> postBassAttachment;
    juce::Label postBassLabel;
    juce::Slider postMidSlider;
    std::unique_ptr<SliderAttachment> postMidAttachment;
    juce::Label postMidLabel;
    juce::Slider postTrebleSlider;
    std::unique_ptr<SliderAttachment> postTrebleAttachment;
    juce::Label postTrebleLabel;

    juce::Slider postGainSlider;
    std::unique_ptr<SliderAttachment> postGainAttachment;
    juce::Label postGainLabel;

    juce::Slider noiseGateThresholdSlider;
    std::unique_ptr<SliderAttachment> noiseGateThresholdAttachment;
    juce::Label noiseGateLabel;

    juce::ComboBox irSelector;
    juce::Label irLabel;
    std::unique_ptr<ComboBoxAttachment> irAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuitarAmpAudioProcessorEditor)
};
