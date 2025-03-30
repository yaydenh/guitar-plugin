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
    IRDir = juce::File::getCurrentWorkingDirectory();
    int numTries = 0;
    while (!IRDir.getChildFile("Resources").exists() && numTries++ < 15)
        IRDir = IRDir.getParentDirectory();

    auto files = IRDir.getChildFile("Resources").findChildFiles(juce::File::TypesOfFileToFind::findFiles, false);
    for (auto& file : files) IRFilenames.add(file.getFileName());
}

void CabSimulator::prepare(juce::dsp::ProcessSpec& spec)
{
    convolver.prepare(spec);
}

void CabSimulator::loadImpulseResponse(juce::String filename)
{
    DBG(filename);
    convolver.loadImpulseResponse(IRDir.getChildFile(filename),
        juce::dsp::Convolution::Stereo::yes,
        juce::dsp::Convolution::Trim::no,
        1024);
}

juce::StringArray CabSimulator::getFilenames()
{
    return IRFilenames;
}
