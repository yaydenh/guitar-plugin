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
    bassFilter.process(context);
    midFilter.process(context);
    trebleFilter.process(context);
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
    bassFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowShelf(sampleRate, 100.0f, 0.7f, juce::Decibels::decibelsToGain(updatedBass));
    midFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, 1000.0f, 1.0f, juce::Decibels::decibelsToGain(updatedMid));
    trebleFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makeHighShelf(sampleRate, 5000.0f, 0.7f, juce::Decibels::decibelsToGain(updatedTreble));
}

void PreampProcessor::setGain(float newGainDb)
{
    float baseGain = 1.0f;
    if (mode == Mode::Clean)   baseGain = 1.5f;
    if (mode == Mode::Crunch)  baseGain = 10.0f;
    if (mode == Mode::Lead)    baseGain = 50.0f;
    DBG("HELLO");
    currentGainDb = newGainDb;
    smoothedGain.setTargetValue(juce::Decibels::decibelsToGain(baseGain + newGainDb));
}

void PreampProcessor::setMode(Mode newMode)
{
    mode = newMode;
}