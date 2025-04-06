/*
  ==============================================================================

    Reverb.h
    Created: 25 Mar 2025 8:14:07pm
    Author:  jayde

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class Reverb
{
public:
    Reverb();

    void prepare(juce::dsp::ProcessSpec& spec);
    void process(juce::AudioBuffer<float>& buffer);

    void setPreDelay(float newPreDelay);
    void setHighCutFreq(float newHighCutFreq);
    void setDiffusion(float newDiffusion);
    void setHighFreqDamping(float newHighFreqDamping);
    void setDecayFactor(float newDecayFactor);
    void setWetDryMix(float newWetDryMix);
    void configure(float newPreDelay,
                   float newHighCutFreq,
                   float newDiffusion,
                   float newHighFreqDamping,
                   float newDecayFactor,
                   float newWetDryMix);

private:
    float indexBuffer(std::deque<float>& buffer, size_t index);
    void pushToBuffer(std::deque<float>& buffer, float value);

    float sampleRate;

    float preDelay         = 0.0f;      // time between hearing input and hearing first reverb
    float highCutFreq      = 20000.0f;  // cutoff for filtering out high freq
    float diffusion        = 0.5f;      // how dense the reverberations are
    float highFreqDamping  = 5e-4;      // how fast high freq reverb decays
    float decayFactor      = 0.5f;      // to make reverb decay faster/slower
    float wetDryMix        = 0.3f;      // ratio of reverb and original sample in output

    // prev input
    std::queue<float> prev;

    // prev input/output for decorrelation all pass filters
    std::vector<std::queue<float>> APInput;
    std::vector<std::queue<float>> APOutput;
    
    // more buffers for all pass filtering
    // deque instead of queue because we want random access too
    std::deque<float> modAP1Input;
    std::deque<float> modAP1Output;
    std::deque<float> modAP2Input;
    std::deque<float> modAP2Output;
    std::deque<float> AP5Input;
    std::deque<float> AP5Output;
    std::deque<float> AP6Input;
    std::deque<float> AP6Output;
};