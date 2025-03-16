/*
  ==============================================================================

    EQProcessor.h
    Created: 16 Mar 2025 4:42:34pm
    Author:  jayde

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class EQProcessor
{
public:
    EQProcessor();
    ~EQProcessor() = default;

    void prepare(juce::dsp::ProcessSpec& spec);
    void process(juce::AudioBuffer<float>& buffer);
    void updateEQ(float bass, float mid, float treble);

private:
    juce::dsp::IIR::Filter<float> bassFilter;
    juce::dsp::IIR::Filter<float> midFilter;
    juce::dsp::IIR::Filter<float> trebleFilter;
    
    float sampleRate = 44100.0f;
};