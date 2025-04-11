/*
  ==============================================================================

    CabSimulator.h
    Created: 22 Mar 2025 10:07:40pm
    Author:  jayde

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"

class CabSimulator
{
public:
    CabSimulator();
    
    void prepare(juce::dsp::ProcessSpec& spec);
    void process(juce::AudioBuffer<float>& buffer);

    void loadImpulseResponse(juce::String filename);
    juce::StringArray getFilenames();

private:
    bool on = false;
    juce::dsp::Convolution convolver;
    juce::File IRDir;
    juce::StringArray IRFilenames;
};