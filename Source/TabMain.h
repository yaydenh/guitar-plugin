/*
  ==============================================================================

    TabMain.h
    Created: 1 Apr 2025 3:15:29am
    Author:  jayde

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

class TabMain : public juce::Component
{
public:
    TabMain(GuitarAmpAudioProcessor& p, juce::AudioProcessorValueTreeState& vts);
    
    void resized() override;

private:
    GuitarAmpAudioProcessor& audioProcessor;

    juce::ToggleButton testToggle;
    std::unique_ptr<ButtonAttachment> testToggleAttachment;
    juce::Label testLabel;

    FrequencyVisualiser preFreq;
    FrequencyVisualiser postFreq;
    juce::AudioVisualiserComponent preVisualiser = juce::AudioVisualiserComponent(1);
    juce::AudioVisualiserComponent postVisualiser = juce::AudioVisualiserComponent(1);
    juce::Label preFreqLabel;
    juce::Label postFreqLabel;
    juce::Label preVisualiserLabel;
    juce::Label postVisualiserLabel;

    juce::ComboBox preampModeSelector;
    std::unique_ptr<ComboBoxAttachment> preampModeAttachment;
    juce::Label preampModeLabel;

    AmpKnob preGain;

    juce::Label preEqLabel;
    AmpKnob toneStackBass;
    AmpKnob toneStackMid;
    AmpKnob toneStackTreble;

    juce::Label postEqLabel;
    AmpKnob postBass;
    AmpKnob postMid;
    AmpKnob postTreble;

    AmpKnob postGain;

    AmpKnob noiseGate;

    juce::ComboBox irSelector;
    juce::Label irLabel;
    std::unique_ptr<ComboBoxAttachment> irAttachment;
};