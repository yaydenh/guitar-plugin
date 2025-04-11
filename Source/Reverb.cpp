/*
  ==============================================================================

    Reverb.cpp
    Created: 25 Mar 2025 8:14:07pm
    Author:  jayde

  ==============================================================================
*/

// algorithm based on:
// https://au.mathworks.com/help/audio/ref/reverberator-system-object.html
// https://ccrma.stanford.edu/~dattorro/EffectDesignPart1.pdf
// https://ccrma.stanford.edu/~dattorro/EffectDesignPart2.pdf

#include "Reverb.h"

Reverb::Reverb()
    : APInput(4, std::queue<float>()),
      APOutput(4, std::queue<float>())
{
}

void Reverb::prepare(juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;
}

void Reverb::process(juce::AudioBuffer<float>& buffer)
{
    auto* leftChannel = buffer.getWritePointer(0);
    auto* rightChannel = buffer.getWritePointer(1);

    juce::AudioBuffer<float> processingBuffer(1, buffer.getNumSamples());
    auto* processingBufferSamples = processingBuffer.getWritePointer(0);

    // constant for lowpass
    const float alpha = std::exp(-2.0f * juce::MathConstants<float>::pi * highCutFreq / sampleRate);

    for (int n = 0; n < buffer.getNumSamples(); n++)
    {
        // convert stereo to mono
        float x = (leftChannel[n] + rightChannel[n]) / 2.0f;

        // predelay
        // maintain a queue of size k, so we can easily get kth previous sample
        const int k = preDelay * sampleRate;
        
        if (prev.size() == k && k != 0)
        {
            x = prev.front();
            prev.pop();
        }
        prev.push(x);

        // low pass - smoothing to remove high freq
        xLowPass = (1.0f - alpha) * x + alpha * xLowPass;
        x = xLowPass;

        processingBufferSamples[n] = x;
    }

    // 4 all pass filters to decorrelate signal
    // y[n] = beta * x[n] + x[n-k] - beta * y[n-k]
    // where k = 142, 107, 379, 277 and beta = diffusion
    std::vector<float> APDelays{ 142, 107, 379, 277 };
    for (int i = 0; i < 4; i++)
    {
        for (int n = 0; n < buffer.getNumSamples(); n++)
        {
            float prevX = 0.0f;
            float prevY = 0.0f;

            // get x[n-k] and y[n-k]
            if (APInput[i].size() == APDelays[i])
            {
                prevX = APInput[i].front();
                APInput[i].pop();
            }
            if (APOutput[i].size() == APDelays[i])
            {
                prevY = APOutput[i].front();
                APOutput[i].pop();
            }

            float y = diffusion * processingBufferSamples[n] + prevX - diffusion * prevY;
            APInput[i].push(processingBufferSamples[n]);
            APOutput[i].push(y);

            processingBufferSamples[n] = y;
        }
    }

    // tank
    for (int n = 0; n < buffer.getNumSamples(); n++)
    {
        float branch1 = processingBufferSamples[n];
        float branch2 = processingBufferSamples[n];

        // add feedback
        branch1 += indexBuffer(AP6Output, 3163);
        branch2 += indexBuffer(AP5Output, 3720);

        // Modulated All Pass Section
        // y[n] = - beta * x[n] + x[n-k] + beta * y[n-k]
        // where beta = diffusion and k[n] = (8/29761) * sampleRate * sin(2pi * n/sampleRate)
        // so k[n] roughly oscillates between -8 to 8
        // cant have negative k so we add max value

        pushToBuffer(modAP1Input, branch1);
        pushToBuffer(modAP2Input, branch2);

        float k = (8.0f / 29761.0f) * sampleRate * std::sin(2.0f * juce::MathConstants<float>::pi * sampleCount / sampleRate);
        k += (8.0f / 29761.0f) * sampleRate;

        // private member sampleCount so we can track a full sampleRate cycle for k oscillator
        sampleCount++;
        if (sampleCount > sampleRate) sampleCount = 0.0f;

        // all pass interpolation for fractional k
        // k is delay in samples so we can't get fractional index from delay line
        float whole;
        float frac = std::modf(k, &whole);
        float coeff = (1.0f - frac) / (1.0f + frac);
        interpIn1 = coeff * (indexBuffer(modAP1Input, whole) - interpIn1) + indexBuffer(modAP1Input, whole + 1);
        interpOut1 = coeff * (indexBuffer(modAP1Output, whole) - interpOut1) + indexBuffer(modAP1Output, whole + 1);
        interpIn2 = coeff * (indexBuffer(modAP2Input, whole) - interpIn2) + indexBuffer(modAP2Input, whole + 1);
        interpOut2 = coeff * (indexBuffer(modAP2Output, whole) - interpOut2) + indexBuffer(modAP2Output, whole + 1);

        // modulated all pass
        branch1 = diffusion * (interpOut1 - branch1) + interpIn1;
        branch2 = diffusion * (interpOut2 - branch2) + interpIn2;

        pushToBuffer(modAP1Output, branch1);
        pushToBuffer(modAP2Output, branch2);

        // Low Pass
        branch1 = indexBuffer(modAP1Output, 4453);
        branch2 = indexBuffer(modAP2Output, 4217);
        LP1 = (1.0f - highFreqDamping) * branch1 + highFreqDamping * LP1;
        LP2 = (1.0f - highFreqDamping) * branch2 + highFreqDamping * LP2;

        // decay factor
        branch1 = LP1 * decayFactor;
        branch2 = LP2 * decayFactor;

        // more all pass
        pushToBuffer(AP5Input, branch1);
        pushToBuffer(AP6Input, branch2);
        branch1 = diffusion * branch1 + indexBuffer(AP5Input, 1800) - diffusion * indexBuffer(AP5Output, 1800);
        branch2 = diffusion * branch2 + indexBuffer(AP6Input, 2656) - diffusion * indexBuffer(AP6Output, 2656);
        pushToBuffer(AP5Output, branch1);
        pushToBuffer(AP6Output, branch2);

        // sum output 
        float outputL = 0.0f;
        float outputR = 0.0f;

        outputR += indexBuffer(modAP1Output, 353);
        outputR += indexBuffer(modAP1Output, 3627);
        outputL -= indexBuffer(modAP1Output, 1990);
        outputR -= indexBuffer(AP5Output, 1228);
        outputL -= indexBuffer(AP5Output, 187);
        outputR += indexBuffer(AP5Output, 2673);
        outputL -= indexBuffer(AP5Output, 1066);
        outputR -= indexBuffer(AP6Output, 121);
        outputL += indexBuffer(AP6Output, 1996);
        outputR -= indexBuffer(AP6Output, 335);
        outputL -= indexBuffer(AP6Output, 1913);
        outputR -= indexBuffer(modAP2Output, 2111);
        outputL += indexBuffer(modAP2Output, 2974);
        outputL += indexBuffer(modAP2Output, 266);
        
        outputL *= 0.6f;
        outputR *= 0.6f;

        // wet-dry blend output
        leftChannel[n] = (1.0f - wetDryMix) * leftChannel[n] + wetDryMix * outputL;
        rightChannel[n] = (1.0f - wetDryMix) * rightChannel[n] + wetDryMix * outputR;
    }   
}

void Reverb::setPreDelay(float newPreDelay)
{
    preDelay = newPreDelay;
}

void Reverb::setHighCutFreq(float newHighCutFreq)
{
    highCutFreq = newHighCutFreq;
}

void Reverb::setDiffusion(float newDiffusion)
{
    diffusion = newDiffusion;
}

void Reverb::setHighFreqDamping(float newHighFreqDamping)
{
    highFreqDamping = newHighFreqDamping;
}

void Reverb::setDecayFactor(float newDecayFactor)
{
    decayFactor = newDecayFactor;
}

void Reverb::setWetDryMix(float newWetDryMix)
{
    wetDryMix = newWetDryMix;
}

void Reverb::configure(float newPreDelay,
                       float newHighCutFreq,
                       float newDiffusion,
                       float newHighFreqDamping,
                       float newDecayFactor,
                       float newWetDryMix)
{
    preDelay = newPreDelay;
    highCutFreq = newHighCutFreq;
    diffusion = newDiffusion;
    highFreqDamping = newHighFreqDamping;
    decayFactor = newDecayFactor;
    wetDryMix = newWetDryMix;
}

// if index out of bounds, return oldest at back
float Reverb::indexBuffer(std::deque<float>& buffer, size_t index)
{
    if (buffer.size() == 0) return 0.0f;
    if (index >= buffer.size()) return buffer.back();
    return buffer[index];
}

// push to front, remove oldest from back when size is 5000
void Reverb::pushToBuffer(std::deque<float>& buffer, float value)
{
    buffer.push_front(value);
    if (buffer.size() == 5000) buffer.pop_back();
}