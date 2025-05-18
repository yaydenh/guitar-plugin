/*
  ==============================================================================

    PreEffects.h
    Created: 18 May 2025 8:45:35pm
    Author:  jayde

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "AmpKnob.h"
#include "Compressor.h"

typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

class TabPreEffects : public juce::Component
{
public:
    TabPreEffects(GuitarAmpAudioProcessor& p, juce::AudioProcessorValueTreeState& vts);

    void resized() override;

private:
    GuitarAmpAudioProcessor& audioProcessor;

    AmpKnob distortionDrive;
    AmpKnob distortionTone;
    AmpKnob distortionLevel;
    juce::ToggleButton distortionOnButton;
    std::unique_ptr<ButtonAttachment> distortionOnAttachment;
    juce::Label distortionLabel;

    AmpKnob overdriveDrive;
    AmpKnob overdriveTone;
    AmpKnob overdriveLevel;
    juce::ToggleButton overdriveOnButton;
    std::unique_ptr<ButtonAttachment> overdriveOnAttachment;
    juce::Label overdriveLabel;

    AmpKnob fuzzSustain;
    AmpKnob fuzzTone;
    AmpKnob fuzzLevel;
    juce::ToggleButton fuzzOnButton;
    std::unique_ptr<ButtonAttachment> fuzzOnAttachment;
    juce::Label fuzzLabel;
};
