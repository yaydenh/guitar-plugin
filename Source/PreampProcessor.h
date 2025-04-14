/*
  ==============================================================================

    PreampProcessor.h
    Created: 16 Mar 2025 7:07:58pm
    Author:  jayde

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <../Source/Compressor.h>

class PreampProcessor
{
public:
    enum class Mode { Clean, Crunch, Lead };

    PreampProcessor();
    ~PreampProcessor() = default;

    void prepare(juce::dsp::ProcessSpec& spec);
    void process(juce::AudioBuffer<float>& buffer);
    void setGain(float newGainDb);
    void setMode(Mode newMode);

    bool test = false;
private:
    int numChannels = 2;
    std::vector<juce::dsp::IIR::Filter<float>> lowFreqFilters;
    std::vector<juce::dsp::IIR::Filter<float>> highFreqFilters;
    std::vector<juce::dsp::IIR::Filter<float>> boost800Hz;
    std::vector<juce::dsp::IIR::Filter<float>> boost5000Hz;

    std::vector<juce::dsp::IIR::Filter<float>> bassFilters;
    std::vector<juce::dsp::IIR::Filter<float>> midFilters;
    std::vector<juce::dsp::IIR::Filter<float>> trebleFilters;

    float sampleRate = 44100.0f;

    juce::dsp::Gain<float> gainProcessor;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> smoothedGain;
    float currentGainDb = 0.0f;

    Compressor compressor;

    Mode mode = Mode::Clean;
};