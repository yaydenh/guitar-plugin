/*
  ==============================================================================

    AmpKnob.h
    Created: 31 Mar 2025 6:43:29pm
    Author:  jayde

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class AmpKnob : public juce::Component
{
public:
    AmpKnob(float min,
            float max,
            float interval,
            juce::String labelText,
            juce::String parameterId,
            juce::AudioProcessorValueTreeState&);

    void paint(juce::Graphics& g) override;
    void resized() override;
    void displayValue(bool showValue);

private:
    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;

    bool valueVisible = true;
    juce::Slider slider;
    juce::Label label;
    std::unique_ptr<SliderAttachment> attachment;
};