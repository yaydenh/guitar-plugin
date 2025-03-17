/*
  ==============================================================================

    EQProcessor.cpp
    Created: 16 Mar 2025 4:42:34pm
    Author:  jayde

  ==============================================================================
*/

#include "EQProcessor.h"

EQProcessor::EQProcessor()
{
}

void EQProcessor::prepare(juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;
    numChannels = spec.numChannels;
    for (int i = 0; i < numChannels; i++)
    {
        bassFilters.push_back(juce::dsp::IIR::Filter<float>());
        bassFilters[i].prepare(spec);
        midFilters.push_back(juce::dsp::IIR::Filter<float>());
        midFilters[i].prepare(spec);
        trebleFilters.push_back(juce::dsp::IIR::Filter<float>());
        trebleFilters[i].prepare(spec);
    }
    updateEQ(0.0f, 0.0f, 0.0f);
}

void EQProcessor::process(juce::AudioBuffer<float>& buffer)
{
    juce::dsp::AudioBlock<float> block(buffer);

    for (int channel = 0; channel < numChannels; channel++)
    {
        juce::dsp::AudioBlock<float> channelBlock = block.getSingleChannelBlock(channel);
        juce::dsp::ProcessContextReplacing<float> channelContext(channelBlock);

        bassFilters[channel].process(channelContext);
        midFilters[channel].process(channelContext);
        trebleFilters[channel].process(channelContext);
    }
}

void EQProcessor::updateEQ(float bass, float mid, float treble)
{
    for (int channel = 0; channel < numChannels; channel++)
    {
        bassFilters[channel].coefficients = juce::dsp::IIR::Coefficients<float>::makeLowShelf(sampleRate, 100.0f, 0.7f, juce::Decibels::decibelsToGain(bass));
        midFilters[channel].coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, 1000.0f, 1.0f, juce::Decibels::decibelsToGain(mid));
        trebleFilters[channel].coefficients = juce::dsp::IIR::Coefficients<float>::makeHighShelf(sampleRate, 8000.0f, 0.7f, juce::Decibels::decibelsToGain(treble));
    }
}