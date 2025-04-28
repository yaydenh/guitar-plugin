/*
  ==============================================================================

    Distortion.h
    Created: 16 Apr 2025 2:53:59pm
    Author:  jayde

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class Distortion
{
public:
    Distortion();

    void prepare(juce::dsp::ProcessSpec& spec);
    void process(juce::AudioBuffer<float>& buffer);

private:
    float sampleRate = 44100.0f;

    // controls
    float distortion = 0.0f;
    float tone       = 0.0f;
    float levelDb    = 0.0f;

    // high passes
    std::vector<float> hp1;
    std::vector<float> hp1Prev;
    float hp1Alpha = 0.0f;
    std::vector<float> hp2;
    std::vector<float> hp2Prev;
    float hp2Alpha = 0.0f;
    std::vector<float> hp3;
    std::vector<float> hp3Prev;
    float hp3Alpha = 0.0f;
    std::vector<float> hp4;
    std::vector<float> hp4Prev;
    float hp4Alpha = 0.0f;

    // low passes
    std::vector<float> lp1;
    float lp1Alpha = 0.0f;
    std::vector<float> lp2;
};