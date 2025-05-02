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
      distortionDrive(0.0f, 1.0f, 0.05f, "Drive", "distortionDrive", vts),
      distortionTone(0.0f, 1.0f, 0.05f, "Tone", "distortionTone", vts),
      distortionLevel(-12.0f, 12.0f, 0.1f, "Level", "distortionLevel", vts),
      overdriveDrive(0.0f, 1.0f, 0.05f, "Drive", "overdriveDrive", vts),
      overdriveTone(0.0f, 1.0f, 0.05f, "Tone", "overdriveTone", vts),
      overdriveLevel(-12.0f, 12.0f, 0.1f, "Level", "overdriveLevel", vts),
      fuzzSustain(0.0f, 1.0f, 0.05f, "Sustain", "fuzzSustain", vts),
      fuzzTone(0.0f, 1.0f, 0.05f, "Tone", "fuzzTone", vts),
      fuzzLevel(-12.0f, 12.0f, 0.1f, "Level", "fuzzLevel", vts)
{
    addAndMakeVisible(compressorBlend);
    addAndMakeVisible(compressorThreshold);
    addAndMakeVisible(compressorAttack);
    addAndMakeVisible(compressorRelease);
    addAndMakeVisible(compressorKneeWidth);
    addAndMakeVisible(compressorRatio);
    addAndMakeVisible(compressorMakeUpGain);
    addAndMakeVisible(compressorOnButton);
    addAndMakeVisible(compressorLabel);
    compressorOnAttachment = std::make_unique<ButtonAttachment>(vts, "compressorOn", compressorOnButton);
    compressorLabel.setText("Compressor", juce::NotificationType::dontSendNotification);

    addAndMakeVisible(reverbPreDelay);
    addAndMakeVisible(reverbHighCutFreq);
    addAndMakeVisible(reverbDiffusion);
    addAndMakeVisible(reverbHighFreqDamping);
    addAndMakeVisible(reverbDecayFactor);
    addAndMakeVisible(reverbWetDryMix);
    addAndMakeVisible(reverbOnButton);
    addAndMakeVisible(reverbLabel);
    reverbOnAttachment = std::make_unique<ButtonAttachment>(vts, "reverbOn", reverbOnButton);
    reverbLabel.setText("Reverb", juce::NotificationType::dontSendNotification);

    addAndMakeVisible(distortionDrive);
    addAndMakeVisible(distortionTone);
    addAndMakeVisible(distortionLevel);
    addAndMakeVisible(distortionOnButton);
    addAndMakeVisible(distortionLabel);
    distortionOnAttachment = std::make_unique<ButtonAttachment>(vts, "distortionOn", distortionOnButton);
    distortionLabel.setText("Distortion", juce::NotificationType::dontSendNotification);

    addAndMakeVisible(overdriveDrive);
    addAndMakeVisible(overdriveTone);
    addAndMakeVisible(overdriveLevel);
    addAndMakeVisible(overdriveOnButton);
    addAndMakeVisible(overdriveLabel);
    overdriveOnAttachment = std::make_unique<ButtonAttachment>(vts, "overdriveOn", overdriveOnButton);
    overdriveLabel.setText("Overdrive", juce::NotificationType::dontSendNotification);

    addAndMakeVisible(fuzzSustain);
    addAndMakeVisible(fuzzTone);
    addAndMakeVisible(fuzzLevel);
    addAndMakeVisible(fuzzOnButton);
    addAndMakeVisible(fuzzLabel);
    fuzzOnAttachment = std::make_unique<ButtonAttachment>(vts, "fussOn", fuzzOnButton);
    fuzzLabel.setText("Fuzz", juce::NotificationType::dontSendNotification);
}

void TabPedals::resized()
{
    auto knobSize = 70;
    auto pedalHeight = knobSize + 30 + 20;
    auto paddingX = 10;
    auto paddingY = 10;
    
    int pedalCount = 0;

    // compressor
    compressorOnButton.setBounds(0, paddingY, 30, 30);
    compressorLabel.setBounds(30, paddingY, 100, 30);

    std::vector<juce::Component*> compressorKnobs = {
        &compressorBlend, &compressorThreshold, &compressorAttack, &compressorRelease,
        &compressorKneeWidth, &compressorRatio, &compressorMakeUpGain
    };

    for (int i = 0; i < compressorKnobs.size(); i++)
        compressorKnobs[i]->setBounds(paddingX + i * knobSize, paddingY + 20, knobSize, knobSize);
    pedalCount++;

    // reverb
    reverbOnButton.setBounds(0, paddingY + pedalCount * pedalHeight, 30, 30);
    reverbLabel.setBounds(30, paddingY + pedalCount * pedalHeight, 100, 30);

    std::vector<juce::Component*> reverbKnobs = {
        &reverbPreDelay, &reverbHighCutFreq, &reverbDiffusion, &reverbHighFreqDamping,
        &reverbDecayFactor, &reverbWetDryMix
    };

    for (int i = 0; i < reverbKnobs.size(); i++)
        reverbKnobs[i]->setBounds(paddingX + i * knobSize, paddingY + 20 + pedalCount * pedalHeight, knobSize, knobSize);
    pedalCount++;

    // distortion
    distortionOnButton.setBounds(0, paddingY + pedalCount * pedalHeight, 30, 30);
    distortionLabel.setBounds(30, paddingY + pedalCount * pedalHeight, 100, 30);

    std::vector<juce::Component*> distortionKnobs = {
        &distortionDrive, & distortionTone, & distortionLevel
    };

    for (int i = 0; i < distortionKnobs.size(); i++)
        distortionKnobs[i]->setBounds(paddingX + i * knobSize, paddingY + 20 + pedalCount * pedalHeight, knobSize, knobSize);
    pedalCount++;

    // overdrive
    overdriveOnButton.setBounds(0, paddingY + pedalCount * pedalHeight, 30, 30);
    overdriveLabel.setBounds(30, paddingY + pedalCount * pedalHeight, 100, 30);

    std::vector<juce::Component*> overdriveKnobs = {
        &overdriveDrive, &overdriveTone, &overdriveLevel
    };

    for (int i = 0; i < overdriveKnobs.size(); i++)
        overdriveKnobs[i]->setBounds(paddingX + i * knobSize, paddingY + 20 + pedalCount * pedalHeight, knobSize, knobSize);
    pedalCount++;

    // fuzz
    fuzzOnButton.setBounds(0, paddingY + pedalCount * pedalHeight, 30, 30);
    fuzzLabel.setBounds(30, paddingY + pedalCount * pedalHeight, 100, 30);

    std::vector<juce::Component*> fuzzKnobs = {
        &fuzzSustain, &fuzzTone, &fuzzLevel
    };

    for (int i = 0; i < fuzzKnobs.size(); i++)
        fuzzKnobs[i]->setBounds(paddingX + i * knobSize, paddingY + 20 + pedalCount * pedalHeight, knobSize, knobSize);
    pedalCount++;

}