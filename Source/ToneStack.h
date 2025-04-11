/*
  ==============================================================================

    ToneStack.h
    Created: 11 Apr 2025 5:47:31pm
    Author:  jayde

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class ToneStack
{
public:
    ToneStack();

    void prepare(juce::dsp::ProcessSpec& spec);
    void process(juce::AudioBuffer<float>& buffer);
    void updateEQ(float newBassDb, float newMidDb, float newTrebleDb);
private:
    float sampleRate;

    float bassDb = 0.0f;
    float midDb = 0.0f;
    float trebleDb = 0.0f;

    std::vector<juce::dsp::IIR::Filter<float>> bassFilters;
    std::vector<juce::dsp::IIR::Filter<float>> midFilters;
    std::vector<juce::dsp::IIR::Filter<float>> trebleFilters;
};