/*
  ==============================================================================

    NoiseGate.cpp
    Created: 19 Mar 2025 3:33:46pm
    Author:  jayde

  ==============================================================================
*/

#include "NoiseGate.h"

NoiseGate::NoiseGate()
{
}

void NoiseGate::prepare(const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;

    // coeff formula:
    // ms * 0.001 * samplingRate = number of samples in attackMs
    // exp(-1 / x) - transitions smoothly from 0 to 1
    attackCoeff = std::exp(-1.0f / (attackMs * 0.001f * spec.sampleRate));
    releaseCoeff = std::exp(-1.0f / (releaseMs * 0.001f * spec.sampleRate));
}

void NoiseGate::process(juce::AudioBuffer<float>& buffer)
{
    for (int channel = 0; channel < buffer.getNumChannels(); channel++)
    {
        auto* samples = buffer.getWritePointer(channel);

        for (int i = 0; i < buffer.getNumSamples(); i++)
        {
            float input = std::abs(samples[i]);
            float inputDb = juce::Decibels::gainToDecibels(input + 1e-6f);
            
            // input above threshold -> open gate
            // hysteresis to prevent rapid open/close at thresholdDb
            if (inputDb > thresholdDb + hysteresisDb)
                // envelope moves to 1 smoothly
                envelope = envelope * attackCoeff + (1.0f - attackCoeff);

            // input below threshold -> close gate
            else if (inputDb < thresholdDb)
                // envelope moves to 0 in exponential decay
                envelope = envelope * releaseCoeff;

            //DBG(envelope);
            samples[i] *= envelope;
        }
    }
}

void NoiseGate::setThreshhold(float newThresholdDb)
{
    thresholdDb = newThresholdDb;
}

void NoiseGate::setParameters(float newAttackMs, float newReleaseMs, float newHysteresisDb)
{
    attackMs = newAttackMs;
    releaseMs = newReleaseMs;
    hysteresisDb = newHysteresisDb;

    attackCoeff = std::exp(-1.0f / (attackMs * 0.001f * sampleRate));
    releaseCoeff = std::exp(-1.0f / (releaseMs * 0.001f * sampleRate));
}