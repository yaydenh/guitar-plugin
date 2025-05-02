/*
  ==============================================================================

    Fuzz.h
    Created: 28 Apr 2025 6:03:16pm
    Author:  jayde

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class Fuzz
{
public:
    Fuzz();
    
    void prepare(juce::dsp::ProcessSpec& spec);
    void process(juce::AudioBuffer<float>& buffer);

private:
    float sampleRate = 44100.0f;

    // controls
    float sustain = 0.0f;
    float tone = 0.0f;
    float levelDb = 0.0f;

    std::vector<float> hp;
    std::vector<float> hpPrev;
    float hpAlpha = 0.0f;

    std::vector<float> lp1;
    float lp1Alpha = 0.0f;

    std::vector<float> lp2;
    
};