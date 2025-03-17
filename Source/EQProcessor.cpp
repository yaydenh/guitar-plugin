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
    bassFilter.prepare(spec);
    midFilter.prepare(spec);
    trebleFilter.prepare(spec);
    bassFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowShelf(sampleRate, 100.0f, 0.7f, juce::Decibels::decibelsToGain(0.0f));
    midFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, 1000.0f, 1.0f, juce::Decibels::decibelsToGain(0.0f));
    trebleFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makeHighShelf(sampleRate, 8000.0f, 0.7f, juce::Decibels::decibelsToGain(0.0f));
    updateEQ(0.0f, 0.0f, 0.0f);
}

void EQProcessor::process(juce::AudioBuffer<float>& buffer)
{
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);

    bassFilter.process(context);
    midFilter.process(context);
    trebleFilter.process(context);
}

void EQProcessor::updateEQ(float bass, float mid, float treble)
{
    bassFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowShelf(sampleRate, 100.0f, 0.7f, juce::Decibels::decibelsToGain(bass));
    midFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, 1000.0f, 1.0f, juce::Decibels::decibelsToGain(mid));
    trebleFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makeHighShelf(sampleRate, 8000.0f, 0.7f, juce::Decibels::decibelsToGain(treble));
}