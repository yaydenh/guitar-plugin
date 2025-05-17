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
    IRDir = IRDir.getChildFile("Resources");

    IRFilenames.add("None");
    auto files = IRDir.findChildFiles(juce::File::TypesOfFileToFind::findFiles, false);
    for (auto& file : files) IRFilenames.add(file.getFileName());
}

void CabSimulator::prepare(juce::dsp::ProcessSpec& spec)
{
    convolver.prepare(spec);
}

void CabSimulator::process(juce::AudioBuffer<float>& buffer)
{
    if (on)
    {
        juce::dsp::AudioBlock<float> block(buffer);
        juce::dsp::ProcessContextReplacing<float> context(block);
        convolver.process(context);
    }
}

void CabSimulator::loadImpulseResponse(juce::String filename)
{
    on = filename != "None";
    if (on)
    {
        convolver.loadImpulseResponse(IRDir.getChildFile(filename),
            juce::dsp::Convolution::Stereo::yes,
            juce::dsp::Convolution::Trim::no,
            1024);
    }
}

juce::StringArray CabSimulator::getFilenames()
{
    return IRFilenames;
}
