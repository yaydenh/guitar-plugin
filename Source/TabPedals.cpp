/*
  ==============================================================================

    TabPedals.cpp
    Created: 1 Apr 2025 4:15:07pm
    Author:  jayde

  ==============================================================================
*/

#include "TabPedals.h"

TabPedals::TabPedals(GuitarAmpAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : audioProcessor(p), valueTreeState(vts),
      compressorLevel(1.0f, 5.0f, 0.1f, "Level", "compressorLevel", vts),
      compressorSustain(1.0f, 5.0f, 0.1f, "Sustain", "compressorSustain", vts),
      compressorBlend(1.0f, 5.0f, 0.1f, "Blend", "compressorBlend", vts)
{
    addAndMakeVisible(compressorLevel);
    addAndMakeVisible(compressorSustain);
    addAndMakeVisible(compressorBlend);
    
    addAndMakeVisible(compressorOnButton);
    compressorOnAttachment = std::make_unique<ButtonAttachment>(valueTreeState, "compressorOn", compressorOnButton);
}

void TabPedals::resized()
{
    compressorLevel.setBounds(10, 10, 100, 100);
    compressorSustain.setBounds(10 + 100, 10, 100, 100);
    compressorBlend.setBounds(10 + 200, 10, 100, 100);
    compressorOnButton.setBounds(0, 0, 30, 30);
}