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

    AmpKnob compressorBlend;
    AmpKnob compressorThreshold;
    AmpKnob compressorAttack;
    AmpKnob compressorRelease;
    AmpKnob compressorKneeWidth;
    AmpKnob compressorRatio;
    AmpKnob compressorMakeUpGain;
    juce::ToggleButton compressorOnButton;
    std::unique_ptr<ButtonAttachment> compressorOnAttachment;

    AmpKnob reverbPreDelay;
    AmpKnob reverbHighCutFreq;
    AmpKnob reverbDiffusion;
    AmpKnob reverbHighFreqDamping;
    AmpKnob reverbDecayFactor;
    AmpKnob reverbWetDryMix;
    juce::ToggleButton reverbOnButton;
    std::unique_ptr<ButtonAttachment> reverbOnAttachment;

    AmpKnob overdriveDrive;
    AmpKnob overdriveTone;
    AmpKnob overdriveLevel;
    juce::ToggleButton overdriveOnButton;
    std::unique_ptr<ButtonAttachment> overdriveOnAttachment;
};