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

}

void Overdrive::process(juce::AudioBuffer<float>& buffer)
{

    for (int channel = 0; channel < buffer.getNumChannels(); channel++)
    {
        auto* samples = buffer.getWritePointer(channel);
        for (int n = 0; n < buffer.getNumSamples(); n++)
        {
            float x = samples[n];

            float alpha = 1.0f;

            // high pass
            // y[n] = a * y[n-1] + a * (x[n-1] - x[n])
            hp1 = alpha * (hp1 + prev - x);
        }
    }
    // high pass with cutoff 15.9 15.6 hz
    // 
    // clipping high pass signal


    // sum clipped signal and original signal


    // more low pass high pass
    // low pass, cutoff 723.4hz, -20db/dec
    // remove harsh high freq

}