/*
  ==============================================================================

    TabPedals.h
    Created: 1 Apr 2025 4:15:07pm
    Author:  jayde

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "AmpKnob.h"
#include "Compressor.h"

typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

class TabPedals : public juce::Component
{
public:
    TabPedals(GuitarAmpAudioProcessor& p, juce::AudioProcessorValueTreeState& vts);

    void resized() override;

private:
    GuitarAmpAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& valueTreeState;

    AmpKnob compressorLevel;
    AmpKnob compressorSustain;
    AmpKnob compressorBlend;
    juce::ToggleButton compressorOnButton;
    std::unique_ptr<ButtonAttachment> compressorOnAttachment;
};