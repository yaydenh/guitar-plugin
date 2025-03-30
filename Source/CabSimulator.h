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
    template <typename ProcessContext>
    void process(const ProcessContext& context) { convolver.process(context); };

    void loadImpulseResponse(juce::String filename);
    juce::StringArray getFilenames();

private:
    juce::dsp::Convolution convolver;
    juce::File IRDir;
    juce::StringArray IRFilenames;
};