/*
  ==============================================================================

    GainProcessor.h
    Created: 14 Mar 2025 2:26:24pm
    Author:  jayde

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class GainProcessor
{
public:
    GainProcessor();
    ~GainProcessor() = default;

    void prepare(const juce::dsp::ProcessSpec& spec);
    void process(juce::AudioBuffer<float>& buffer);
    void setGain(float newGainDb);
    float getGain() const;

private:
    juce::dsp::Gain<float> gainProcessor;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> smoothedGain;
    float currentGainDb = 0.0f;
};