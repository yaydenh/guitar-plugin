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
    enum class Mode { Clean, Crunch, Lead };

    PreampProcessor();
    ~PreampProcessor() = default;

    void prepare(juce::dsp::ProcessSpec& spec);
    void process(juce::AudioBuffer<float>& buffer);
    void updateEQ(float bass, float mid, float treble);
    void setGain(float newGainDb);
    void setMode(Mode newMode);

private:
    juce::dsp::IIR::Filter<float> bassFilter;
    juce::dsp::IIR::Filter<float> midFilter;
    juce::dsp::IIR::Filter<float> trebleFilter;

    float sampleRate = 44100.0f;

    juce::dsp::Gain<float> gainProcessor;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> smoothedGain;
    float currentGainDb = 0.0f;

    Mode mode = Mode::Clean;
};