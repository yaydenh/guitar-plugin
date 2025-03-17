/*
  ==============================================================================

    PreampProcessor.h
    Created: 16 Mar 2025 7:07:58pm
    Author:  jayde

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class PreampProcessor
{
public:
    PreampProcessor();
    ~PreampProcessor() = default;

    void prepare(juce::dsp::ProcessSpec& spec);
    void process(juce::AudioBuffer<float>& buffer);
    void updateEQ(float bass, float mid, float treble);
    void setGain(float newGainDb);

private:
    int numChannels;

    std::vector<juce::dsp::IIR::Filter<float>> bassFilters;
    std::vector<juce::dsp::IIR::Filter<float>> midFilters;
    std::vector<juce::dsp::IIR::Filter<float>> trebleFilters;

    float sampleRate = 44100.0f;

    juce::dsp::Gain<float> gainProcessor;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> smoothedGain;
    float currentGainDb = 0.0f;
};