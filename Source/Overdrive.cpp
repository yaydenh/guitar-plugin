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
    hp1.resize(spec.numChannels, 0.0f);
    hp2.resize(spec.numChannels, 0.0f);
    hp1Prev.resize(spec.numChannels, 0.0f);
    hp2Prev.resize(spec.numChannels, 0.0f);
    lp1.resize(spec.numChannels, 0.0f);
    lp2.resize(spec.numChannels, 0.0f);
}

void Overdrive::process(juce::AudioBuffer<float>& buffer)
{
    juce::ScopedNoDenormals noDenormals;

    for (int channel = 0; channel < buffer.getNumChannels(); channel++)
    {
        auto* samples = buffer.getWritePointer(channel);
        for (int n = 0; n < buffer.getNumSamples(); n++)
        {
            float x = samples[n];
            float y = x;

            // CLIPPING AMPLIFYING STAGE

            // high passes
            // y[n] = a * y[n-1] + a * (x[n] - x[n-1])

            // cutoff 15.9 Hz
            hp1[channel] = hp1Alpha * (hp1[channel] + y - hp1Prev[channel]);
            hp1Prev[channel] = y;
            y = hp1[channel];

            // cutoff 720 Hz
            hp2[channel] = hp2Alpha * (hp2[channel] + y - hp2Prev[channel]);
            hp2Prev[channel] = y;
            y = hp2[channel];

            // amplify and clip
            // gain from non inverting op amp
            y *= 1.0f + ((51000.0f + 500000.0f * overdrive) / 4700.0f);
            y = std::tanh(y); // change tanh to approximation for performance?

            // low pass
            // cutoff = 1 / 2piRC where R varies from overdrive
            // y[n] = (1 - a) * y[n-1] + a * x[n]
            const double lp1Cutoff = 1.0 / (2.0 * juce::MathConstants<double>::pi * 51.0 * 0.000000000001 * (51000.0 + 500000.0 * overdrive));
            const float lp1Alpha = 1.0f / (1.0f + (sampleRate / (2.0f * juce::MathConstants<float>::pi * lp1Cutoff)));
            lp1[channel] += lp1Alpha * (y - lp1[channel]);
            y = lp1[channel];

            // sum with input signal
            y += hp1[channel];

            // TONE AND VOLUME STAGE

            // low pass 
            // cutoff between 200 to 5000 depending on tone knob
            const float lp2Cutoff = 200.0f * std::powf(5000.0f / 200.0f, tone);
            const float lp2Alpha = 1.0f / (1.0f + (sampleRate / (2.0f * juce::MathConstants<float>::pi * lp2Cutoff)));
            lp2[channel] += lp2Alpha * (y - lp2[channel]);
            y = lp2[channel];

            // volume 
            y *= juce::Decibels::decibelsToGain(levelDb);

            // lowpass, cutoff 723.4 Hz
            //lp2 = (1 - lp2Alpha) * lp2 + lp2Alpha * y;
            //y = lp2;

            samples[n] = y;
        }
    }
}

void Overdrive::configure(float newOverdrive, float newTone, float newLevelDb)
{
    overdrive = newOverdrive;
    tone = newTone;
    levelDb = newLevelDb;
}