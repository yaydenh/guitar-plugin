/*
  ==============================================================================

    Overdrive.cpp
    Created: 22 Mar 2025 11:19:47pm
    Author:  jayde

  ==============================================================================
*/

// https://www.electrosmash.com/tube-screamer-analysis

#include "Overdrive.h"

Overdrive::Overdrive()
{
}

void Overdrive::prepare(juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;
    hp1Alpha = 1.0f / (1.0f + (2.0f * juce::MathConstants<float>::pi * 15.9f / sampleRate));
    hp2Alpha = 1.0f / (1.0f + (2.0f * juce::MathConstants<float>::pi * 15.6f / sampleRate));
    lpAlpha = 1.0f / (1.0f + (sampleRate / (2.0f * juce::MathConstants<float>::pi * 723.4f));
}

void Overdrive::process(juce::AudioBuffer<float>& buffer)
{

    for (int channel = 0; channel < buffer.getNumChannels(); channel++)
    {
        auto* samples = buffer.getWritePointer(channel);
        for (int n = 0; n < buffer.getNumSamples(); n++)
        {
            float x = samples[n];
            float y = 0.0f;

            // high passes
            // y[n] = a * y[n-1] + a * (x[n-1] - x[n])

            // cutoff 15.9 Hz
            hp1 = hp1Alpha * (hp1 + hp1Prev - x);
            hp1Prev = x;

            // cutoff 15.6 Hz
            hp2 = hp2Alpha * (hp2 + hp1 - hp2Prev);
            hp2Prev = hp1;

            // clipping
            // change tanh to approximation for performance?
            y = std::tanh(hp2);

            // sum with input signal
            y += x;

            // low pass, cutoff 723.4hz, -20db/dec
            // y[n] = (1 - a) * y[n-1] + a * x[n]
            lp = (1 - lpAlpha) * lp + lpAlpha * y;
            y = lp;

            samples[n] = y;
        }
    }
}