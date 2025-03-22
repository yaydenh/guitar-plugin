/*
  ==============================================================================

    CabSimulator.cpp
    Created: 22 Mar 2025 10:07:40pm
    Author:  jayde

  ==============================================================================
*/

#include "CabSimulator.h"

CabSimulator::CabSimulator()
{
    auto dir = juce::File::getCurrentWorkingDirectory();
    int numTries = 0;
    while (!dir.getChildFile("Resources").exists() && numTries++ < 15)
        dir = dir.getParentDirectory();

    convolver.loadImpulseResponse(dir.getChildFile("Resources").getChildFile("guitar_amp.wav"),
        juce::dsp::Convolution::Stereo::yes,
        juce::dsp::Convolution::Trim::no,
        1024);
}

void CabSimulator::prepare(juce::dsp::ProcessSpec& spec)
{
    convolver.prepare(spec);
}
