/*
  ==============================================================================

    AmpKnob.cpp
    Created: 31 Mar 2025 6:43:29pm
    Author:  jayde

  ==============================================================================
*/

#include "AmpKnob.h"

AmpKnob::AmpKnob(float min,
                 float max,
                 float interval,
                 juce::String labelText,
                 juce::String parameterId,
                 juce::AudioProcessorValueTreeState& vts)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setRange(min, max, interval);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 50, 20);
    slider.onValueChange = [this] { repaint(); };

    label.setText(labelText, juce::NotificationType::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.attachToComponent(&slider, false);
    label.setInterceptsMouseClicks(false, false);

    attachment = std::make_unique<SliderAttachment>(vts, parameterId, slider);

    addAndMakeVisible(slider);
}

void AmpKnob::paint(juce::Graphics& g)
{
    if (valueVisible)
    {
        float value = slider.getValue();
        g.setColour(juce::Colours::white);
        g.setFont(15.0f);
        g.drawFittedText(juce::String(value, 1), getLocalBounds(), juce::Justification::centred, 1);
    }
}

void AmpKnob::resized()
{
    auto bounds = getLocalBounds();
    slider.setBounds(bounds);
    label.setBounds(bounds.removeFromBottom(20));
}

void AmpKnob::displayValue(bool showValue)
{
    valueVisible = showValue;
}


