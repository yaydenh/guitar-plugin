/*
  ==============================================================================

    DIstortionProcessor.cpp
    Created: 14 Mar 2025 7:28:42pm
    Author:  jayde

  ==============================================================================
*/

#include "DIstortionProcessor.h"

#include <iostream>
#include <fstream>

DistortionProcessor::DistortionProcessor()
{
    distortionType = Type::SoftClip;
}

void DistortionProcessor::setType(Type newType)
{
    distortionType = newType;
}

void DistortionProcessor::setDrive(float newDrive)
{
    drive = newDrive;
}

void DistortionProcessor::process(juce::AudioBuffer<float>& buffer)
{
    for (int channel = 0; channel < buffer.getNumChannels(); channel++)
    {
        auto* samples = buffer.getWritePointer(channel);
        for (int i = 0; i < buffer.getNumSamples(); i++)
        {
            samples[i] = applyDistortion(samples[i] * drive);
        }
    }
}

float DistortionProcessor::applyDistortion(float sample)
{
    //std::ofstream log;
    //log.open("log.txt");
    //log << "yuh " << (int)distortionType << std::endl;
    //log.close();
    float newSample = sample;
    float gain = 1.0f + (std::abs(sample) * 0.1f);

    switch (distortionType)
    {
        case Type::SoftClip:
            newSample = juce::jlimit(-1.0f, 1.0f, sample - (sample * sample * sample * 0.5f));
            break;
        case Type::HardClip:
            newSample = juce::jlimit(-0.4f, 0.4f, sample);
            break;

        case Type::WaveShaper:
            newSample = std::tanh(sample);
            break;

        case Type::Fuzz:
            if (sample > 0.9f) newSample = 1.0f;
            else if (sample < -0.9f) newSample = -1.0f;
            else newSample = sample;
            newSample = std::sin(newSample);
            break;

        default:
            newSample = sample;
            break;
    }


    return newSample;
}