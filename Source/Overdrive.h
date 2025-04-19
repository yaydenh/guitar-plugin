/*
  ==============================================================================

    Overdrive.h
    Created: 22 Mar 2025 11:19:47pm
    Author:  jayde

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class Overdrive
{
public:
    Overdrive();

    void prepare(juce::dsp::ProcessSpec& spec);
    void process(juce::AudioBuffer<float>& buffer);

private:
    float sampleRate = 44100.0f;

    // controls
    float overdrive;
    float tone;
    float levelDb;

    // high/low pass variables
    float hp1 = 0.0f;
    float hp2 = 0.0f;
    float hp1Prev = 0.0f;
    float hp2Prev = 0.0f;
    float hp1Alpha = 0.0f;
    float hp2Alpha = 0.0f;

    float lp1 = 0.0f;
    float lp2 = 0.0f;
};