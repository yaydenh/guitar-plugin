/*
  ==============================================================================

    TabPedals.cpp
    Created: 1 Apr 2025 4:15:07pm
    Author:  jayde

  ==============================================================================
*/

#include "TabPedals.h"

TabPedals::TabPedals(GuitarAmpAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : audioProcessor(p),
      compressorBlend(0.0f, 1.0f, 0.05f, "Blend", "compressorBlend", vts),
      compressorThreshold(-50.0f, 0.0f, 0.1f, "Threshold", "compressorThreshold", vts),
      compressorAttack(0.5f, 50.0f, 0.1f, "Attack", "compressorAttack", vts),
      compressorRelease(50.0f, 150.0f, 0.1f, "Release", "compressorRelease", vts),
      compressorKneeWidth(0.0f, 20.0f, 0.1f, "KneeWidth", "compressorKneeWidth", vts),
      compressorRatio(1.0f, 50.0f, 0.1f, "Ratio", "compressorRatio", vts),
      compressorMakeUpGain(-10.0f, 20.0f, 0.1f, "Level", "compressorMakeUpGain", vts),
      reverbPreDelay(0.0f, 1.0f, 0.001f, "PreDelay", "reverbPreDelay", vts),
      reverbHighCutFreq(20.0f, 20000.0f, 1.0f, "HighCutFreq", "reverbHighCutFreq", vts),
      reverbDiffusion(0.0f, 1.0f, 0.05f, "Diffusion", "reverbDiffusion", vts),
      reverbHighFreqDamping(0.0f, 1.0f, 0.05f, "HighFreqDamping", "reverbHighFreqDamping", vts),
      reverbDecayFactor(0.0f, 1.0f, 0.05f, "DecayFactor", "reverbDecayFactor", vts),
      reverbWetDryMix(0.0f, 1.0f, 0.05f, "WetDryMix", "reverbWetDryMix", vts),
      overdriveDrive(0.0f, 1.0f, 0.05f, "Drive", "overdriveDrive", vts),
      overdriveTone(0.0f, 1.0f, 0.05f, "Tone", "overdriveTone", vts),
      overdriveLevel(-12.0f, 12.0f, 0.1f, "Level", "overdriveLevel", vts)
{
    addAndMakeVisible(compressorBlend);
    addAndMakeVisible(compressorThreshold);
    addAndMakeVisible(compressorAttack);
    addAndMakeVisible(compressorRelease);
    addAndMakeVisible(compressorKneeWidth);
    addAndMakeVisible(compressorRatio);
    addAndMakeVisible(compressorMakeUpGain);
    addAndMakeVisible(compressorOnButton);
    compressorOnAttachment = std::make_unique<ButtonAttachment>(vts, "compressorOn", compressorOnButton);

    addAndMakeVisible(reverbPreDelay);
    addAndMakeVisible(reverbHighCutFreq);
    addAndMakeVisible(reverbDiffusion);
    addAndMakeVisible(reverbHighFreqDamping);
    addAndMakeVisible(reverbDecayFactor);
    addAndMakeVisible(reverbWetDryMix);
    addAndMakeVisible(reverbOnButton);
    reverbOnAttachment = std::make_unique<ButtonAttachment>(vts, "reverbOn", reverbOnButton);

    addAndMakeVisible(overdriveDrive);
    addAndMakeVisible(overdriveTone);
    addAndMakeVisible(overdriveLevel);
    addAndMakeVisible(overdriveOnButton);
    overdriveOnAttachment = std::make_unique<ButtonAttachment>(vts, "overdriveOn", overdriveOnButton);
}

void TabPedals::resized()
{
    auto knobSize = 70;
    auto padding = 10;
    
    // compressor
    compressorOnButton.setBounds(0, 0, 30, 30);

    std::vector<juce::Component*> compressorKnobs = {
        &compressorBlend, &compressorThreshold, &compressorAttack, &compressorRelease,
        &compressorKneeWidth, &compressorRatio, &compressorMakeUpGain
    };

    for (int i = 0; i < compressorKnobs.size(); i++)
        compressorKnobs[i]->setBounds(padding + i * knobSize, padding, knobSize, knobSize);

    // reverb
    reverbOnButton.setBounds(0, knobSize, 30, 30);

    std::vector<juce::Component*> reverbKnobs = {
        &reverbPreDelay, &reverbHighCutFreq, &reverbDiffusion, &reverbHighFreqDamping,
        &reverbDecayFactor, &reverbWetDryMix
    };

    // overdrive
    overdriveOnButton.setBounds(0, 2 * knobSize, 30, 30);

    for (int i = 0; i < reverbKnobs.size(); i++)
        reverbKnobs[i]->setBounds(padding + i * knobSize, padding + knobSize, knobSize, knobSize);

    std::vector<juce::Component*> overdriveKnobs = {
        &overdriveDrive, &overdriveTone, &overdriveLevel
    };

    for (int i = 0; i < overdriveKnobs.size(); i++)
        overdriveKnobs[i]->setBounds(padding + i * knobSize, padding + 2 * knobSize, knobSize, knobSize);
}