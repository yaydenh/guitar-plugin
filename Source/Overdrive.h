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
    void configure(float newOverdrive, float newTone, float newLevelDb);

private:
    float sampleRate = 44100.0f;

    // controls
    float overdrive;
    float tone;
    float levelDb;

    // high/low pass variables
    std::vector<float> hp1;
    std::vector<float> hp2;
    std::vector<float> hp1Prev;
    std::vector<float> hp2Prev;
    float hp1Alpha = 0.0f;
    float hp2Alpha = 0.0f;

    std::vector<float> lp1;
    std::vector<float> lp2;
};