/*
  ==============================================================================

    Fuzz.cpp
    Created: 28 Apr 2025 6:03:16pm
    Author:  jayde

  ==============================================================================
*/

#include "Fuzz.h"

Fuzz::Fuzz()
{
}

void Fuzz::prepare(juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;
    hp.resize(spec.numChannels, 0.0f);
    hpPrev.resize(spec.numChannels, 0.0f);
    lp1.resize(spec.numChannels, 0.0f);
    lp1Alpha = 1.0f / (1.0f + (sampleRate / (2.0f * juce::MathConstants<float>::pi * 1200.0f)));
    lp2.resize(spec.numChannels, 0.0f);
}

void Fuzz::process(juce::AudioBuffer<float>& buffer)
{
    for (int channel = 0; channel < buffer.getNumChannels(); channel++)
    {
        auto* samples = buffer.getWritePointer(channel);
        for (int n = 0; n < buffer.getNumSamples(); n++)
        {
            float x = samples[n];
            float y = x;

            // boost 16.7db
            y *= juce::Decibels::decibelsToGain(16.7f);

            // high pass 3.8 Hz
            // y[n] = a * y[n-1] + a * (x[n] - x[n-1])
            hp[channel] = hpAlpha * (hp[channel] + y - hpPrev[channel]);
            hpPrev[channel] = y;
            y = hp[channel];

            // low pass 1200 Hz
            // y[n] = (1 - a) * y[n-1] + a * x[n]
            lp1[channel] += lp1Alpha * (y - lp1[channel]);
            y = lp1[channel];

            // sustain control
            y *= 0.5f + sustain;

            // boost 23db
            y *= juce::Decibels::decibelsToGain(23.0f);

            // soft clipping * 0.6
            y = 0.6f * std::tanh(y);

            // boost 25db
            y *= juce::Decibels::decibelsToGain(25.0f);

            // soft clipping * 0.6, again to make it harsher
            y = 0.6f * std::tanh(y);

            // TONE AND VOLUME STAGE

            // low pass 
            // cutoff between 200 to 5000 depending on tone knob
            const float lp2Cutoff = 200.0f * std::powf(5000.0f / 200.0f, tone);
            const float lp2Alpha = 1.0f / (1.0f + (sampleRate / (2.0f * juce::MathConstants<float>::pi * lp2Cutoff)));
            lp2[channel] += lp2Alpha * (y - lp2[channel]);
            y = lp2[channel];

            // volume 
            y *= juce::Decibels::decibelsToGain(levelDb);

            samples[n] = y;
        }
    }
}

void Fuzz::configure(float newSustain, float newTone, float newLevelDb)
{
    sustain = newSustain;
    tone = newTone;
    levelDb = newLevelDb;
}