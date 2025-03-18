/*
  ==============================================================================

    PreampProcessor.cpp
    Created: 16 Mar 2025 7:07:58pm
    Author:  jayde

  ==============================================================================
*/

#include "PreampProcessor.h"

PreampProcessor::PreampProcessor()
{
    gainProcessor.setGainDecibels(currentGainDb);
    smoothedGain.reset(44100, 0.05f);
}

void PreampProcessor::prepare(juce::dsp::ProcessSpec& spec)
{
    gainProcessor.prepare(spec);
    smoothedGain.reset(spec.sampleRate, 0.05f);
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
    setGain(0);
}

void PreampProcessor::process(juce::AudioBuffer<float>& buffer)
{
    smoothedGain.applyGain(buffer, buffer.getNumSamples());
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);

    // pre gain
    gainProcessor.process(context);

    // pre EQ
    for (int channel = 0; channel < numChannels; channel++)
    {
        juce::dsp::AudioBlock<float> channelBlock = block.getSingleChannelBlock(channel);
        juce::dsp::ProcessContextReplacing<float> channelContext(channelBlock);

        bassFilters[channel].process(channelContext);
        midFilters[channel].process(channelContext);
        trebleFilters[channel].process(channelContext);
    }

    // clipping
    for (int channel = 0; channel < numChannels; channel++)
    {
        auto* samples = buffer.getWritePointer(channel);
        for (int i = 0; i < buffer.getNumSamples(); i++)
        {
            if (mode == Mode::Clean)
            {
                // no clipping?
            }
            if (mode == Mode::Crunch)
            {
                samples[i] = juce::jlimit(-0.8f, 0.8f, samples[i] + 0.2f * std::tanh(samples[i]));
            }
            if (mode == Mode::Lead)
            {
                samples[i] = juce::jlimit(-0.6f, 0.6f, samples[i]);
            }
        }
    }

    // post EQ?
    
    // normalise volume
    // not really normalising at the moment, but makes the volume not unbearable
    buffer.applyGain(juce::Decibels::decibelsToGain(-1 * currentGainDb));
    //if (mode == Mode::Clean)        buffer.applyGain(juce::Decibels::decibelsToGain(-0.0f));
    //else if (mode == Mode::Crunch)  buffer.applyGain(juce::Decibels::decibelsToGain(-5.0f));
    //else if (mode == Mode::Lead)    buffer.applyGain(juce::Decibels::decibelsToGain(-10.0f));
}

void PreampProcessor::updateEQ(float bass, float mid, float treble)
{
    float updatedBass = bass, updatedMid = mid, updatedTreble = treble;
    if (mode == Mode::Clean)
    {
        updatedBass += 0.0f;
        updatedMid += 2.0f;
        updatedTreble += 4.0f;
    }
    else if (mode == Mode::Crunch)
    {
        updatedBass += -2.0f;
        updatedMid += 4.0f;
        updatedTreble += 2.0f;
    }
    else if (mode == Mode::Lead)
    {
        updatedBass += -4.0f;
        updatedMid += 6.0f;
        updatedTreble += 1.0f;
    }
    for (int channel = 0; channel < numChannels; channel++)
    {
        bassFilters[channel].coefficients = juce::dsp::IIR::Coefficients<float>::makeLowShelf(sampleRate, 100.0f, 0.7f, juce::Decibels::decibelsToGain(updatedBass));
        midFilters[channel].coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, 1000.0f, 1.0f, juce::Decibels::decibelsToGain(updatedMid));
        trebleFilters[channel].coefficients = juce::dsp::IIR::Coefficients<float>::makeHighShelf(sampleRate, 8000.0f, 0.7f, juce::Decibels::decibelsToGain(updatedTreble));
    }
}

void PreampProcessor::setGain(float newGainDb)
{
    float baseGain = 1.0f;
    if (mode == Mode::Clean)        baseGain = 1.5f;
    else if (mode == Mode::Crunch)  baseGain = 10.0f;
    else if (mode == Mode::Lead)    baseGain = 20.0f;
    currentGainDb = baseGain + newGainDb;
    smoothedGain.setTargetValue(juce::Decibels::decibelsToGain(baseGain + newGainDb));
}

void PreampProcessor::setMode(Mode newMode)
{
    mode = newMode;
}