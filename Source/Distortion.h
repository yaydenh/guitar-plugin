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
    float drive;

};