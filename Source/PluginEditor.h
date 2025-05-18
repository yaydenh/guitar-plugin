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
#include "TabMain.h"
#include "TabPedals.h"
#include "TabPreEffects.h"
#include "TabPostEffects.h"

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
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    GuitarAmpAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& valueTreeState;

    juce::TabbedComponent tabs;

    TabMain mainTab;
    TabPreEffects preEffectsTab;
    TabPostEffects postEffectsTab;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuitarAmpAudioProcessorEditor)
};
