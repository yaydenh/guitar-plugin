/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
typedef juce::AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;

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
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    GuitarAmpAudioProcessor& audioProcessor;

    juce::AudioProcessorValueTreeState& valueTreeState;

    juce::ComboBox preampModeSelector;
    std::unique_ptr<ComboBoxAttachment> preampModeAttachment;

    juce::Slider preGainSlider;
    std::unique_ptr<SliderAttachment> preGainAttachment;

    juce::Slider preBassSlider;
    std::unique_ptr<SliderAttachment> preBassAttachment;
    juce::Slider preMidSlider;
    std::unique_ptr<SliderAttachment> preMidAttachment;
    juce::Slider preTrebleSlider;
    std::unique_ptr<SliderAttachment> preTrebleAttachment;

    juce::ComboBox distortionTypeSelector;
    std::unique_ptr<ComboBoxAttachment> distortionTypeAttachment;
    juce::Slider driveSlider;
    std::unique_ptr<SliderAttachment> driveAttachment;

    juce::Slider postBassSlider;
    std::unique_ptr<SliderAttachment> postBassAttachment;
    juce::Slider postMidSlider;
    std::unique_ptr<SliderAttachment> postMidAttachment;
    juce::Slider postTrebleSlider;
    std::unique_ptr<SliderAttachment> postTrebleAttachment;

    juce::Slider postGainSlider;
    std::unique_ptr<SliderAttachment> postGainAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuitarAmpAudioProcessorEditor)
};
