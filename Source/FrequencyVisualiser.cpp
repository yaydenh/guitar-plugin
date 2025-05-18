/*
  ==============================================================================

    FrequencyVisualiser.cpp
    Created: 18 Mar 2025 6:16:22pm
    Author:  jayde

  ==============================================================================
*/

#include "FrequencyVisualiser.h"

FrequencyVisualiser::FrequencyVisualiser()
    : fft(fftOrder), window(fftSize, juce::dsp::WindowingFunction<float>::hann)
{
    startTimer(100);
}

void FrequencyVisualiser::paint(juce::Graphics& g)
{
    drawFrame(g);
}

void FrequencyVisualiser::pushSample(float sample)
{
    // if fifo full, set flag to say next frame should be rendered
    if (fifoIndex == fftSize)
    {
        if (!nextFFTBlockReady)
        {
            juce::zeromem(fftData, sizeof(fftData));
            memcpy(fftData, fifo, sizeof(fifo));
            nextFFTBlockReady = true;
        }
        fifoIndex = 0;
    }
    fifo[fifoIndex++] = sample;
}

void FrequencyVisualiser::timerCallback()
{
    if (nextFFTBlockReady)
    {
        drawNextFrameOfSpectrum();
        nextFFTBlockReady = false;
        repaint();
    }
}

void FrequencyVisualiser::drawNextFrameOfSpectrum()
{
    // apply windowing function
    window.multiplyWithWindowingTable(fftData, fftSize);

    // render fft data
    fft.performFrequencyOnlyForwardTransform(fftData);
    auto minDb = -100.0f;
    auto maxDb = 0.0f;

    for (int i = 0; i < scopeSize; i++)
    {
        auto skewedProportionX = 1.0f - std::exp(std::log(1.0f - (float)i / (float)scopeSize) * 0.2f);
        auto fftDataIndex = juce::jlimit(0, fftSize / 2, (int)(skewedProportionX * (float)fftSize * 0.5f));
        auto level = juce::jmap(juce::jlimit(minDb, maxDb, juce::Decibels::gainToDecibels(fftData[fftDataIndex]) - juce::Decibels::gainToDecibels((float)fftSize)),
            minDb,
            maxDb,
            0.0f,
            1.0f);
        scopeData[i] = level;
    }
}

void FrequencyVisualiser::drawFrame(juce::Graphics& g)
{
    for (int i = 0; i < scopeSize; i++)
    {
        auto width = 320;
        auto height = 200;
        g.drawLine({(float) juce::jmap(i - 1, 0, scopeSize - 1, 0, width),
            juce::jmap(scopeData[i - 1], 0.0f, 1.0f, (float) height, 0.0f),
            (float) juce::jmap(i, 0, scopeSize - 1, 0, width),
            juce::jmap(scopeData[i], 0.0f, 1.0f, (float) height, 0.0f)
        });
    }
}