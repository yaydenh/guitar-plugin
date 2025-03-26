/*
  ==============================================================================

    Compressor.cpp
    Created: 25 Mar 2025 9:51:10pm
    Author:  jayde

  ==============================================================================
*/

#include "Compressor.h"

// https://eecs.qmul.ac.uk/~josh/documents/2012/GiannoulisMassbergReiss-dynamicrangecompression-JAES2012.pdf


// Gain Stage
// y =  x + c + M
// where y is output, x is input, c is control, M is makeup (all in decibels)

// Gain Computer - generates control c
// Compression Ratio:
// R = (xG - T) / (yG - T) for xG > T
// where T is threshold
// In words, this is the recipricol of the slope of input above the threshold
// yG = xG                  if xG <= T   (output is same as input)
//      T + (xG - T)/R      if xG > T    
// Knee Stuff Too

// Level Detection

void Compressor::process(juce::AudioBuffer<float>& buffer)
{
    for (int channel = 0; channel < buffer.getNumChannels(); channel++)
    {
        auto* samples = buffer.getWritePointer(channel);
        float yL = 0, y1 = 0;
        for (int i = 0; i < buffer.getNumSamples(); i++)
        {
            float xG = 20 * std::log10f(std::abs(samples[i]));
            float yG = xG <= thresholdDb ? xG : thresholdDb + (xG - thresholdDb) / ratio;

            float xL = xG - yG;
            y1 = std::max(xL, release * y1 + (1 - release) * xL);
            yL = attack * yL + (1 - attack) * y1;

            samples[i] += makeUpGain - yL;

        }
    }
}