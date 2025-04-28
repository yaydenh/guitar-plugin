/*
  ==============================================================================

    Distortion.cpp
    Created: 16 Apr 2025 2:53:59pm
    Author:  jayde

  ==============================================================================
*/

// https://www.electrosmash.com/boss-ds1-analysis

#include "Distortion.h"

Distortion::Distortion()
{

}

void Distortion::prepare(juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;
    hp1.resize(spec.numChannels, 0.0f);
    hp2.resize(spec.numChannels, 0.0f);
    hp3.resize(spec.numChannels, 0.0f);
    hp4.resize(spec.numChannels, 0.0f);
    hp1Prev.resize(spec.numChannels, 0.0f);
    hp2Prev.resize(spec.numChannels, 0.0f);
    hp3Prev.resize(spec.numChannels, 0.0f);
    hp4Prev.resize(spec.numChannels, 0.0f);
    hp1Alpha = 1.0f / (1.0f + (2.0f * juce::MathConstants<float>::pi * 3.3f / sampleRate));
    hp2Alpha = 1.0f / (1.0f + (2.0f * juce::MathConstants<float>::pi * 33.0f / sampleRate));
    hp2Alpha = 1.0f / (1.0f + (2.0f * juce::MathConstants<float>::pi * 23.0f / sampleRate));
    hp2Alpha = 1.0f / (1.0f + (2.0f * juce::MathConstants<float>::pi * 73.0f / sampleRate));

    lp1.resize(spec.numChannels, 0.0f);
    lp2.resize(spec.numChannels, 0.0f);
    lp1Alpha = 1.0f / (1.0f + (sampleRate / (2.0f * juce::MathConstants<float>::pi * 7200.0f)));
}

void Distortion::process(juce::AudioBuffer<float>& buffer)
{
    for (int channel = 0; channel < buffer.getNumChannels(); channel++)
    {
        auto* samples = buffer.getWritePointer(channel);

        for (int n = 0; n < buffer.getNumSamples(); n++)
        {
            float x = samples[n];
            float y = x;

            // TRANSISTOR BOOSTING STAGE

            // 2 high passes
            // remove excess bass before distortion
            // y[n] = a * y[n-1] + a * (x[n] - x[n-1])
            // cutoff 3.3Hz
            hp1[channel] = hp1Alpha * (hp1[channel] + y - hp1Prev[channel]);
            hp1Prev[channel] = y;
            y = hp1[channel];

            // cutoff 33Hz
            hp2[channel] = hp2Alpha * (hp2[channel] + y - hp2Prev[channel]);
            hp2Prev[channel] = y;
            y = hp2[channel];

            // boost by 35db
            y *= juce::Decibels::decibelsToGain(35.0f);

            // OP AMP GAIN STAGE
            
            // high pass 23Hz
            hp3[channel] = hp3Alpha * (hp3[channel] + y - hp3Prev[channel]);
            hp3Prev[channel] = y;
            y = hp3[channel];

            // non inverting op amp boost
            // gain = 1 + (100000 * distortion) / 4700
            y *= 1.0f + (100000.0f * distortion) / 4700.0f;

            // hard clipping
            bool clipped = true;
            if (y > 0.7f) y = 0.7f;
            else if (y < -0.7f) y = -0.7f;
            else clipped = false;

            // high pass 73Hz
            hp4[channel] = hp4Alpha * (hp4[channel] + y - hp4Prev[channel]);
            hp4Prev[channel] = y;
            y = hp4[channel];

            // low pass 7200Hz
            // but only if not clipping
            lp1[channel] += lp1Alpha * (y - lp1[channel]);
            if (!clipped) y = lp1[channel];

            // TONE AND VOLUME

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

