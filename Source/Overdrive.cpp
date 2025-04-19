/*
  ==============================================================================

    Overdrive.cpp
    Created: 22 Mar 2025 11:19:47pm
    Author:  jayde

  ==============================================================================
*/

// https://www.electrosmash.com/tube-screamer-analysis
// https://ccrma.stanford.edu/~dtyeh/papers/DavidYehThesissinglesided.pdf

#include "Overdrive.h"

Overdrive::Overdrive()
{
}

void Overdrive::prepare(juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;
    hp1Alpha = 1.0f / (1.0f + (2.0f * juce::MathConstants<float>::pi * 15.9f / sampleRate));
    hp2Alpha = 1.0f / (1.0f + (2.0f * juce::MathConstants<float>::pi * 720.0f / sampleRate));
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

            // CLIPPING AMPLIFYING STAGE

            // high passes
            // y[n] = a * y[n-1] + a * (x[n-1] - x[n])

            // cutoff 15.9 Hz
            hp1 = hp1Alpha * (hp1 + hp1Prev - x);
            hp1Prev = x;

            // cutoff 720 Hz
            hp2 = hp2Alpha * (hp2 + hp1 - hp2Prev);
            hp2Prev = hp1;

            // amplify and clip
            y *= 1.0f + ((51000.0f + 500000.0f * overdrive) / 4700.0f);
            y = std::tanh(hp2); // change tanh to approximation for performance?

            // low pass
            // cutoff = 1 / 2piRC where R varies from overdrive
            // y[n] = (1 - a) * y[n-1] + a * x[n]
            const float lp1Cutoff = 1.0f / (2.0f * juce::MathConstants<float>::pi * 51 * 0.000001f * (51000.0f + 500000.0f * overdrive));
            const float lp1Alpha = 1.0f / (1.0f + (sampleRate / (2.0f * juce::MathConstants<float>::pi * lp1Cutoff)));
            lp1 = (1.0f - lp1Alpha) * lp1 + lp1Alpha * y;
            y = lp1;

            // sum with input signal after first high pass
            y += hp1;

            // TONE AND VOLUME STAGE

            // low pass 
            // cutoff between 200 to 5000 depending on tone knob
            const float lp2cutoff = 200.0f * std::powf(5000.0f / 200.0f, tone);
            const float lp2Alpha = 1.0f / (1.0f + (sampleRate / (2.0f * juce::MathConstants<float>::pi * lp2Cutoff)));
            lp2 = (1.0f - lp2Alpha) * lp2 + lp2Alpha * y;
            y = lp2;

            // volume 
            y *= juce::Decibels::decibelsToGain(levelDb);

            // lowpass, cutoff 723.4 Hz
            //lp2 = (1 - lp2Alpha) * lp2 + lp2Alpha * y;
            //y = lp2;

            samples[n] = y;
        }
    }
}