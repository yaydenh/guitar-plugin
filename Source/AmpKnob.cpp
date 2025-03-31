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
                 juce::String paramId,
                 juce::AudioProcessorValueTreeState& vts)
    : valueTreeState(vts)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setRange(min, max, interval);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 50, 20);
    slider.onValueChange = [this] { repaint(); };

    label.setText(labelText, juce::NotificationType::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.attachToComponent(&slider, false);
    label.setInterceptsMouseClicks(false, false);

    attachment = std::make_unique<SliderAttachment>(valueTreeState, paramId, slider);

    addAndMakeVisible(slider);
}

void AmpKnob::paint(juce::Graphics& g)
{
    float value = slider.getValue();
    g.setColour(juce::Colours::white);
    g.setFont(15.0f);
    g.drawFittedText(juce::String(value, 1), getLocalBounds(), juce::Justification::centred, 1);
}

void AmpKnob::resized()
{
    auto bounds = getLocalBounds();
    slider.setBounds(bounds);
    label.setBounds(bounds.removeFromBottom(20));
}

//bool AmpKnob::hitTest(int x, int y)
//{
//    auto width = slider.getBounds().getWidth();
//    auto quarter = width / 4;
//    auto sliderClickArea = slider.getBounds().removeFromLeft(quarter).removeFromRight(quarter);
//    return sliderClickArea.contains(x, y);
//}

