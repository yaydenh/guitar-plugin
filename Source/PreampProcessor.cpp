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
    bassFilter.prepare(spec);
    midFilter.prepare(spec);
    trebleFilter.prepare(spec);
    sampleRate = spec.sampleRate;
    updateEQ(0.0f, 0.0f, 0.0f);
}

void PreampProcessor::process(juce::AudioBuffer<float>& buffer)
{
    smoothedGain.applyGain(buffer, buffer.getNumSamples());

    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);

    // pre gain
    gainProcessor.process(context);

    // pre EQ
    bassFilter.process(context);
    midFilter.process(context);
    trebleFilter.process(context);
}

void PreampProcessor::updateEQ(float bass, float mid, float treble)
{
    bassFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowShelf(sampleRate, 100.0f, 0.7f, juce::Decibels::decibelsToGain(bass));
    midFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, 1000.0f, 1.0f, juce::Decibels::decibelsToGain(mid));
    trebleFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makeHighShelf(sampleRate, 5000.0f, 0.7f, juce::Decibels::decibelsToGain(treble));
}

void PreampProcessor::setGain(float newGainDb)
{
    smoothedGain.setTargetValue(juce::Decibels::decibelsToGain(newGainDb));
}