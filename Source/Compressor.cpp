/*
  ==============================================================================

    Compressor.cpp
    Created: 25 Mar 2025 9:51:10pm
    Author:  jayde

  ==============================================================================
*/

#include "Compressor.h"

// https://eecs.qmul.ac.uk/~josh/documents/2012/GiannoulisMassbergReiss-dynamicrangecompression-JAES2012.pdf

Compressor::Compressor(float thresholdDb, float attackMs, float releaseMs, float kneeWidth, float ratio, float makeUpGain)
    :   thresholdDb(thresholdDb),
        attackMs(attackMs),
        releaseMs(releaseMs),
        kneeWidth(kneeWidth),
        ratio(ratio),
        makeUpGain(makeUpGain)
{
}

void Compressor::prepare(juce::dsp::ProcessSpec& spec)
{
    attack = std::exp(-1 / (attackMs * 0.001f * spec.sampleRate));
    release = std::exp(-1 / (releaseMs * 0.001f * spec.sampleRate));
}

void Compressor::process(juce::AudioBuffer<float>& buffer)
{
    for (int channel = 0; channel < buffer.getNumChannels(); channel++)
    {
        auto* samples = buffer.getWritePointer(channel);
        float yL = 0, y1 = 0;
        for (int i = 0; i < buffer.getNumSamples(); i++)
        {
            // convert sample to decibel
            float xG = 20 * std::log10f(std::abs(samples[i]) + 1e-6f);

            // Compression Ratio:
            // R = (xG - T) / (yG - T) for xG > T
            // where T is threshold
            // In words, this is the recipricol of the slope of input above the threshold
            // yG = xG                              if xG - T < W/2     (output is same as input)
            //      xG + (1/R-1)(xG-T+W/2)^2 / 2W   if |xG - T| <= W/2  (smoothing within knee width of threshold)
            //      T + (xG - T)/R                  if xG - T > W/2     (compressed)
            float yG = xG;
            if (2 * std::abs(xG - thresholdDb) <= kneeWidth)
            {
                yG = xG + (1 / ratio - 1) * std::powf(xG - thresholdDb + kneeWidth / 2, 2) / (2 * kneeWidth);
            }
            else if (2 * (xG - thresholdDb) > kneeWidth)
            {
                yG = thresholdDb + (xG - thresholdDb) / ratio;
            }

            // xL = how much was compressed
            float xL = xG - yG;

            // Peak detection for smoothing:

            // tracks peaks of signal
            // if new peak xL, set y1 to xL (fast attack)
            // else if xL is smaller, decay based on release factor (slow release)
            y1 = std::max(xL, release * y1 + (1 - release) * xL);

            // smoothing with attack coefficient - how aggressive compressor is applied
            yL = attack * yL + (1 - attack) * y1;

            samples[i] *= juce::Decibels::decibelsToGain(makeUpGain - yL);

        }
    }
}