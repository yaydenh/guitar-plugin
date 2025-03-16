/*
  ==============================================================================

    DIstortionProcessor.h
    Created: 14 Mar 2025 7:28:42pm
    Author:  jayde

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class DistortionProcessor
{
public:
    enum class Type { SoftClip, HardClip, WaveShaper, Fuzz, None };

    DistortionProcessor();
    ~DistortionProcessor() = default;

    void setType(Type newType);
    void setDrive(float newDrive);
    void process(juce::AudioBuffer<float>& buffer);

private:
    Type distortionType = Type::SoftClip;
    float drive = 1.0f;

    float applyDistortion(float sample);
};