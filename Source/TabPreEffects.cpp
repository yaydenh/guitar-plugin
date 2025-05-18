/*
  ==============================================================================

    PreEffects.cpp
    Created: 18 May 2025 8:45:35pm
    Author:  jayde

  ==============================================================================
*/

#include "TabPreEffects.h"

TabPreEffects::TabPreEffects(GuitarAmpAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : audioProcessor(p),
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
    fuzzOnAttachment = std::make_unique<ButtonAttachment>(vts, "fuzzOn", fuzzOnButton);
    fuzzLabel.setText("Fuzz", juce::NotificationType::dontSendNotification);
}

void TabPreEffects::resized()
{
    auto knobSize = 80;
    auto pedalHeight = knobSize + 30 + 20;
    auto paddingX = 10;
    auto paddingY = 10;

    int pedalCount = 0;

    // distortion
    distortionOnButton.setBounds(0, paddingY + pedalCount * pedalHeight, 30, 30);
    distortionLabel.setBounds(30, paddingY + pedalCount * pedalHeight, 100, 30);

    std::vector<juce::Component*> distortionKnobs = {
        &distortionDrive, &distortionTone, &distortionLevel
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