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
    Compressor();

    void prepare(juce::dsp::ProcessSpec& spec);
    void process(juce::AudioBuffer<float>& buffer);

private:
    float thresholdDb;      // level above which compression starts
    float attack;           // how fast compression starts
    float release;          // how fast compression stops
    float kneeWidth;        // idk
    float ratio;            // i/o ratios for input above threshold i.e. amount of compression
    float makeUpGain;       // increase gain to match input after compressing

    // hold
    // side chain filtering
    // look ahead
};