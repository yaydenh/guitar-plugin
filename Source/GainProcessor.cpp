/*
  ==============================================================================

    GainProcessor.cpp
    Created: 14 Mar 2025 2:26:24pm
    Author:  jayde

  ==============================================================================
*/

#include "GainProcessor.h"

GainProcessor::GainProcessor()
{
    gainProcessor.setGainDecibels(currentGainDb);
    smoothedGain.reset(44100, 0.05f);
}

void GainProcessor::prepare(const juce::dsp::ProcessSpec& spec)
{
    gainProcessor.prepare(spec);
    smoothedGain.reset(spec.sampleRate, 0.05f);
}

void GainProcessor::process(juce::AudioBuffer<float>& buffer)
{
    smoothedGain.applyGain(buffer, buffer.getNumSamples());
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    gainProcessor.process(context);
}

void GainProcessor::setGain(float newGainDb)
{
    currentGainDb = newGainDb;
    smoothedGain.setTargetValue(juce::Decibels::decibelsToGain(currentGainDb));
    //gainProcessor.setGainDecibels(currentGainDb);
}

float GainProcessor::getGain() const
{
    return smoothedGain.getCurrentValue();
}