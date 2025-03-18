/*
  ==============================================================================

    FrequencyVisualiser.h
    Created: 18 Mar 2025 6:16:22pm
    Author:  jayde

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class FrequencyVisualiser : public juce::Component, public juce::Timer
{
public:
    enum {
        fftOrder = 10,
        fftSize = 1 << fftOrder, // 2 ^ 10 = 1024
        scopeSize = 512,         // points in graph
    };

    FrequencyVisualiser();

    void paint(juce::Graphics& g) override;
    void pushSample(float sample);
    void timerCallback() override;

private:
    juce::dsp::FFT fft;
    juce::dsp::WindowingFunction<float> window;
    float fifo[fftSize];
    float fftData[2 * fftSize];
    int fifoIndex = 0;
    bool nextFFTBlockReady = false;
    float scopeData[scopeSize];

    void drawNextFrameOfSpectrum();
    void drawFrame(juce::Graphics& g);
};