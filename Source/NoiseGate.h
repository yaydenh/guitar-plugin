/*
  ==============================================================================

    NoiseGate.h
    Created: 19 Mar 2025 3:33:46pm
    Author:  jayde

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class NoiseGate
{
public:
    NoiseGate();

    void prepare(const juce::dsp::ProcessSpec& spec);
    void process(juce::AudioBuffer<float>& buffer);
    void setThreshhold(float newThreshold);
    void setParameters(float newAttackMs, float newReleaseMs, float newHysteresisDb);
private:
    double sampleRate = 0;

    float thresholdDb = 0.0f;
    float hysteresisDb = 10.0f;

    float attackMs = 50.0f;
    float attackCoeff = 0.0f;
    float releaseMs = 150.0f;
    float releaseCoeff = 0.0f;
};