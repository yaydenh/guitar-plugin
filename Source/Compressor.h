/*
  ==============================================================================

    Compressor.h
    Created: 25 Mar 2025 9:51:10pm
    Author:  jayde

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class Compressor
{
public:
    Compressor() = default;
    Compressor(float thresholdDb, float attackMs, float releaseMs, float kneeWidth, float ratio, float makeUpGain);

    void prepare(juce::dsp::ProcessSpec& spec);
    void process(juce::AudioBuffer<float>& buffer);

    void setThresholdDb(float newThresholdDb);
    void setAttackMs(float newAttackMs);
    void setReleaseMs(float newReleaseMs);
    void setKneeWidth(float newKneeWidth);
    void setRatio(float newRatio);
    void setMakeUpGain(float newMakeUpGain);
    void configure(float newThresholdDb,
                   float newAttackMs,
                   float newReleaseMs,
                   float newKneeWidth,
                   float newRatio,
                   float newMakeUpGain,
                   float newBlend);
private:
    float sampleRate = 0.0f;

    std::vector<juce::dsp::IIR::Filter<float>> filterHighFreq;

    float thresholdDb = -20.0f; // level above which compression starts
    float attackMs    =  10.0f; // how fast compression starts
    float attack      =   0.0f;
    float releaseMs   = 150.0f; // how fast compression stops
    float release     =   0.0f;
    float kneeWidth   =   6.0f; // how smooth the compression is around the threshold
    float ratio       =   2.0f; // i/o ratios for input above threshold i.e. amount of compression
    float makeUpGain  =   6.0f; // increase gain to match input after compressing
    float blend       =   1.0f;

    // to add:
    // hold
    // side chain filtering
    // look ahead
};