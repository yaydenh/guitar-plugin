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
        lowFreqFilters.push_back(
            juce::dsp::IIR::Filter<float>(juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, 70.0f, 0.7f))
        );
        lowFreqFilters[i].prepare(spec);

        boost800Hz.push_back(
            juce::dsp::IIR::Filter<float>(juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, 800.0f, 1.5f, juce::Decibels::decibelsToGain(6.0f)))
        );
        boost800Hz[i].prepare(spec);

        boost5000Hz.push_back(
            juce::dsp::IIR::Filter<float>(juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, 5000.0f, 5.0f, juce::Decibels::decibelsToGain(6.0f)))
        );
        boost5000Hz[i].prepare(spec);

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

    // filter out low freq - prevent muddiness and noise
    for (int channel = 0; channel < numChannels; channel++)
    {
        juce::dsp::AudioBlock<float> channelBlock = block.getSingleChannelBlock(channel);
        juce::dsp::ProcessContextReplacing<float> channelContext(channelBlock);
        lowFreqFilters[channel].process(channelContext);
    }

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
                samples[i] = juce::jlimit(-1.0f, 1.0f, samples[i] - 0.3f * std::powf(samples[i], 3));
            }
            if (mode == Mode::Lead)
            {
                samples[i] = juce::jlimit(-0.8f, 0.8f, samples[i]);
            }
        }
    }

    // post EQ?
    
    // boost 800hz
    for (int channel = 0; channel < numChannels; channel++)
    {
        juce::dsp::AudioBlock<float> channelBlock = block.getSingleChannelBlock(channel);
        juce::dsp::ProcessContextReplacing<float> channelContext(channelBlock);
        boost800Hz[channel].process(channelContext);
        if (test) boost5000Hz[channel].process(channelContext);
    }

    // normalise volume
    // not really normalising at the moment, but makes the volume not unbearable
    buffer.applyGain(1 / sqrt(currentGainDb));
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