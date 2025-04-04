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
    sampleRate = spec.sampleRate;
    attack = std::exp(-1.0f / (attackMs * 0.001f * spec.sampleRate));
    release = std::exp(-1.0f / (releaseMs * 0.001f * spec.sampleRate));
    for (auto i = 0; i < spec.numChannels; i++) {
        filterHighFreq.push_back(juce::dsp::IIR::Filter<float>(juce::dsp::IIR::Coefficients<float>::makeLowPass(spec.sampleRate, 10.0f, 1.0f)));
        filterHighFreq[i].prepare(spec);
    }
}

void Compressor::process(juce::AudioBuffer<float>& buffer)
{

    //juce::dsp::AudioBlock<float> block0(buffer);
    //for (auto channel = 0; channel < buffer.getNumChannels(); channel++)
    //{
    //    juce::dsp::AudioBlock<float> channelBlock = block0.getSingleChannelBlock(channel);
    //    juce::dsp::ProcessContextReplacing<float> channelContext(channelBlock);
    //    filterHighFreq[channel].process(channelContext);
    //}

    float M = 0.0f;
    if (2.0f * std::abs(0.0f - thresholdDb) <= kneeWidth)
    {
        M = 0.0f + (1.0f / ratio - 1.0f) * std::powf(0.0f - thresholdDb + kneeWidth / 2.0f, 2) / (2.0f * kneeWidth);
    }
    else if (2.0f * (0.0f - thresholdDb) > kneeWidth)
    {
        M = thresholdDb + (0.0f - thresholdDb) / ratio;
    }
    M = -M;

    for (auto channel = 0; channel < buffer.getNumChannels(); channel++)
    {
        auto* samples = buffer.getWritePointer(channel);
        float yL = 0, y1 = 0;
        for (int i = 0; i < buffer.getNumSamples(); i++)
        {
            // convert sample to decibel
            float xG = 20 * std::log10f(std::max(std::abs(samples[i]), 1e-8f));

            // Compression Ratio:
            // R = (xG - T) / (yG - T) for xG > T
            // where T is threshold
            // In words, this is the recipricol of the slope of input above the threshold
            // yG = xG                              if xG - T < W/2     (output is same as input)
            //      xG + (1/R-1)(xG-T+W/2)^2 / 2W   if |xG - T| <= W/2  (smoothing within knee width of threshold)
            //      T + (xG - T)/R                  if xG - T > W/2     (compressed)
            float yG = xG;
            if (2.0f * std::abs(xG - thresholdDb) <= kneeWidth)
            {
                yG = xG + (1.0f / ratio - 1.0f) * std::powf(xG - thresholdDb + kneeWidth / 2.0f, 2) / (2.0f * kneeWidth);
            }
            else if (2.0f * (xG - thresholdDb) > kneeWidth)
            {
                yG = thresholdDb + (xG - thresholdDb) / ratio;
            }

            // xL = how much was compressed
            float xL = xG - yG; 

            // Peak detection for smoothing:

            // tracks peaks of signal
            // if new peak xL, set y1 to xL (fast attack)
            // else if xL is smaller, decay based on release factor (slow release)
            y1 = std::max(xL, release * y1 + (1.0f - release) * xL);

            // smoothing with attack coefficient - how aggressive compressor is applied
            yL = attack * yL + (1.0f - attack) * y1;

            float dry = samples[i];
            float wet = samples[i] * juce::Decibels::decibelsToGain(M + makeUpGain - yL);
            samples[i] = (1.0f - blend) * dry + blend * wet;

        }
    }
}

void Compressor::setThresholdDb(float newThresholdDb)
{
    thresholdDb = newThresholdDb;
}

void Compressor::setAttackMs(float newAttackMs)
{
    attackMs = newAttackMs;
    attack = std::exp(-1 / (attackMs * 0.001f * sampleRate));
}

void Compressor::setReleaseMs(float newReleaseMs)
{
    releaseMs = newReleaseMs;
    release = std::exp(-1 / (releaseMs * 0.001f * sampleRate));
}

void Compressor::setKneeWidth(float newKneeWidth)
{
    kneeWidth = newKneeWidth;
}

void Compressor::setRatio(float newRatio)
{
    ratio = newRatio;
}

void Compressor::setMakeUpGain(float newMakeUpGain)
{
    makeUpGain = newMakeUpGain;
}

void Compressor::configure(float newThresholdDb,
                           float newAttackMs,
                           float newReleaseMs,
                           float newKneeWidth,
                           float newRatio,
                           float newMakeUpGain,
                           float newBlend)
{
    thresholdDb = newThresholdDb;
    attackMs = newAttackMs;
    releaseMs = newReleaseMs;
    kneeWidth = newKneeWidth;
    ratio = newRatio;
    makeUpGain = newMakeUpGain;
    blend = newBlend;
}