/*
  ==============================================================================

    PostEffects.h
    Created: 18 May 2025 8:45:26pm
    Author:  jayde

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "AmpKnob.h"
#include "Compressor.h"

typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

class TabPostEffects : public juce::Component
{
public:
    TabPostEffects(GuitarAmpAudioProcessor& p, juce::AudioProcessorValueTreeState& vts);

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
    juce::Label compressorLabel;

    AmpKnob reverbPreDelay;
    AmpKnob reverbHighCutFreq;
    AmpKnob reverbDiffusion;
    AmpKnob reverbHighFreqDamping;
    AmpKnob reverbDecayFactor;
    AmpKnob reverbWetDryMix;
    juce::ToggleButton reverbOnButton;
    std::unique_ptr<ButtonAttachment> reverbOnAttachment;
    juce::Label reverbLabel;
};