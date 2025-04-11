/*
  ==============================================================================

    ToneStack.cpp
    Created: 11 Apr 2025 5:47:31pm
    Author:  jayde

  ==============================================================================
*/

#include "ToneStack.h"

ToneStack::ToneStack()
{
}

void ToneStack::prepare(juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;
    for (int channel = 0; channel < spec.numChannels; channel++)
    {
        bassFilters.push_back(
            juce::dsp::IIR::Filter<float>(juce::dsp::IIR::Coefficients<float>::makeLowShelf(sampleRate, 100.0f, 0.7, 1.0f))
        );
        bassFilters[channel].prepare(spec);

        midFilters.push_back(
            juce::dsp::IIR::Filter<float>(juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, 1000.0f, 0.7f, 1.0f))
        );
        midFilters[channel].prepare(spec);

        trebleFilters.push_back(
            juce::dsp::IIR::Filter<float>(juce::dsp::IIR::Coefficients<float>::makeHighShelf(sampleRate, 5000.0f, 0.7, 1.0f))
        );
        trebleFilters[channel].prepare(spec);
    }
}

void ToneStack::process(juce::AudioBuffer<float>& buffer)
{
    juce::dsp::AudioBlock<float> block(buffer);

    for (int channel = 0; channel < buffer.getNumChannels(); channel++)
    {
        juce::dsp::AudioBlock<float> channelBlock = block.getSingleChannelBlock(channel);
        juce::dsp::ProcessContextReplacing<float> channelContext(channelBlock);

        bassFilters[channel].process(channelContext);
        midFilters[channel].process(channelContext);
        trebleFilters[channel].process(channelContext);
    }
}

void ToneStack::updateEQ(float newBassDb, float newMidDb, float newTrebleDb)
{
    // update filters if value is changed
    if (bassDb != newBassDb)
    {
        for (int channel = 0; channel < bassFilters.size(); channel++)
        {
            auto bassGain = juce::Decibels::decibelsToGain(newBassDb);
            auto coeffs = juce::dsp::IIR::Coefficients<float>::makeLowShelf(sampleRate, 100.0f, 0.7f, bassGain);
            bassFilters[channel].coefficients = coeffs;
        }
    }

    if (midDb != newMidDb)
    {
        for (int channel = 0; channel < midFilters.size(); channel++)
        {
            auto midGain = juce::Decibels::decibelsToGain(newMidDb);
            auto coeffs = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, 1000.0f, 0.7f, midGain);
            midFilters[channel].coefficients = coeffs;
        }
    }

    if (trebleDb != newTrebleDb)
    {
        for (int channel = 0; channel < bassFilters.size(); channel++)
        {
            auto trebleGain = juce::Decibels::decibelsToGain(newTrebleDb);
            auto coeffs = juce::dsp::IIR::Coefficients<float>::makeHighShelf(sampleRate, 5000.0f, 0.7, trebleGain);
            trebleFilters[channel].coefficients = coeffs;
        }
    }

    bassDb = newBassDb;
    midDb = newMidDb;
    trebleDb = newTrebleDb;
}