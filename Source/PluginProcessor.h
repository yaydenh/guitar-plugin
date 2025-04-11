/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "PreampProcessor.h"
#include "ToneStack.h"
#include "GainProcessor.h"
#include "DistortionProcessor.h"
#include "EQProcessor.h"
#include "FrequencyVisualiser.h"
#include "NoiseGate.h"
#include "CabSimulator.h"
#include "Compressor.h"
#include "Reverb.h"

//==============================================================================
/**
*/
class GuitarAmpAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    GuitarAmpAudioProcessor();
    ~GuitarAmpAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    void setPreFrequencyVisualiser(FrequencyVisualiser* f) { preFreq.reset(f); };
    void setPostFrequencyVisualiser(FrequencyVisualiser* f) { postFreq.reset(f); };
    void setPreWaveformVisualiser(juce::AudioVisualiserComponent* v) { preVisualiser.reset(v); };
    void setPostWaveformVisualiser(juce::AudioVisualiserComponent* v) { postVisualiser.reset(v); };
    void setImpulseResponse(juce::String filename) { cabSim.loadImpulseResponse(filename); };
    juce::StringArray getImpulseResponseFilenames() { return cabSim.getFilenames(); };

    Compressor& getCompressor() { return compressor; };

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    juce::AudioProcessorValueTreeState parameters;

    std::unique_ptr<FrequencyVisualiser> preFreq = std::make_unique<FrequencyVisualiser>();
    std::unique_ptr<FrequencyVisualiser> postFreq = std::make_unique<FrequencyVisualiser>();
    std::unique_ptr<juce::AudioVisualiserComponent> preVisualiser = std::make_unique<juce::AudioVisualiserComponent>(1);
    std::unique_ptr<juce::AudioVisualiserComponent> postVisualiser = std::make_unique<juce::AudioVisualiserComponent>(1);

    PreampProcessor preamp;
    ToneStack toneStack;
    DistortionProcessor distortion;
    EQProcessor eq;
    GainProcessor gain;
    NoiseGate noiseGate;
    CabSimulator cabSim;

    // pedals
    Compressor compressor;
    Reverb reverb;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuitarAmpAudioProcessor)
};
