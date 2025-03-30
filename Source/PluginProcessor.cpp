/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#include <iostream>
#include <fstream>

//==============================================================================
GuitarAmpAudioProcessor::GuitarAmpAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    ),
    parameters(*this, nullptr, "parameters",
        {
            std::make_unique<juce::AudioParameterBool>("test", "test", false),
            std::make_unique<juce::AudioParameterChoice>("preampMode", "preampMode",
                juce::StringArray { "Clean", "Crunch", "Lead" }, 0
            ),
            std::make_unique<juce::AudioParameterFloat>("preGain", "preGain", -24.0f, 24.0f, 0.0f),
            std::make_unique<juce::AudioParameterFloat>("preBassEQ", "preBassEQ", -12.0f, 12.0f, 0.0f),
            std::make_unique<juce::AudioParameterFloat>("preMidEQ", "preMidEQ", -12.0f, 12.0f, 0.0f),
            std::make_unique<juce::AudioParameterFloat>("preTrebleEQ", "preTrebleEQ", -12.0f, 12.0f, 0.0f),
            std::make_unique<juce::AudioParameterFloat>("drive", "Drive", 1.0f, 10.0f, 1.0f),
            std::make_unique<juce::AudioParameterChoice>("distortionType", "Distortion Type",
                juce::StringArray { "Soft Clip", "Hard Clip", "WaveShaper", "Fuzz", "None"}, 0
            ),
            std::make_unique<juce::AudioParameterFloat>("postBassEQ", "postBassEQ", -12.0f, 12.0f, 0.0f),
            std::make_unique<juce::AudioParameterFloat>("postMidEQ", "postMidEQ", -12.0f, 12.0f, 0.0f),
            std::make_unique<juce::AudioParameterFloat>("postTrebleEQ", "postTrebleEQ", -12.0f, 12.0f, 0.0f),
            std::make_unique<juce::AudioParameterFloat>("postGain", "postGain", -24.0f, 24.0f, 0.0f),
            std::make_unique<juce::AudioParameterFloat>("noiseGateThreshold", "noiseGateThreshold", -96.0f, 0.0f, -96.0f),
            //std::make_unique<juce::AudioParameterChoice>("irChoice", "IRChoice", cabSim.getFilenames(), 0),
        })
#endif
{
}

GuitarAmpAudioProcessor::~GuitarAmpAudioProcessor()
{
}

//==============================================================================
const juce::String GuitarAmpAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool GuitarAmpAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool GuitarAmpAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool GuitarAmpAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double GuitarAmpAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int GuitarAmpAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int GuitarAmpAudioProcessor::getCurrentProgram()
{
    return 0;
}

void GuitarAmpAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String GuitarAmpAudioProcessor::getProgramName (int index)
{
    return {};
}

void GuitarAmpAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void GuitarAmpAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    juce::dsp::ProcessSpec spec{ sampleRate, (juce::uint32)samplesPerBlock, getTotalNumOutputChannels() };
    preamp.prepare(spec);
    eq.prepare(spec);
    gain.prepare(spec);
    noiseGate.prepare(spec);
    cabSim.prepare(spec);
}

void GuitarAmpAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool GuitarAmpAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void GuitarAmpAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
    }

    // copy right channel to left channel because i dont know how to handle mono input
    auto* rightChannel = buffer.getReadPointer(1);
    auto* leftChannel = buffer.getWritePointer(0);

    for (int sample = 0; sample < buffer.getNumSamples(); sample++)
    {
        leftChannel[sample] = rightChannel[sample];

        // pre freq visualiser
        preFreq->pushSample(rightChannel[sample]);
    }

    bool test = parameters.getParameterAsValue("test").getValue();

    // pre waveform visualiser
    preVisualiser->pushBuffer(buffer);

    // preamp
    preamp.test = test;
    int preampMode = parameters.getParameterAsValue("preampMode").getValue();
    preamp.setMode(static_cast<PreampProcessor::Mode>(preampMode));
    float preampGain = *parameters.getRawParameterValue("preGain");
    preamp.setGain(preampGain);
    float preampBass = *parameters.getRawParameterValue("preBassEQ");
    float preampMid = *parameters.getRawParameterValue("preMidEQ");
    float preampTreble = *parameters.getRawParameterValue("preTrebleEQ");
    preamp.updateEQ(preampBass, preampMid, preampTreble);
    preamp.process(buffer);

    // distortion
    float driveValue = *parameters.getRawParameterValue("drive");
    distortion.setDrive(driveValue);
    int distortionChoice = parameters.getParameterAsValue("distortionType").getValue();
    distortion.setType(static_cast<DistortionProcessor::Type>(distortionChoice));
    distortion.process(buffer);

    // cab sim
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    cabSim.process(context);

    // post EQ
    float postBass = *parameters.getRawParameterValue("postBassEQ");
    float postMid = *parameters.getRawParameterValue("postMidEQ");
    float postTreble = *parameters.getRawParameterValue("postTrebleEQ");
    eq.updateEQ(postBass, postMid, postTreble);
    eq.process(buffer);

    // noise gate
    float noiseGateThreshold = *parameters.getRawParameterValue("noiseGateThreshold");
    noiseGate.setThreshhold(noiseGateThreshold);
    noiseGate.process(buffer);

    // post freq visualiser
    auto* outputSamples = buffer.getWritePointer(0);
    for (int i = 0; i < buffer.getNumSamples(); i++)
    {
        postFreq->pushSample(outputSamples[i]);
    }

    // post waveform visualiser
    postVisualiser->pushBuffer(buffer);

    // post gain
    float postGain = *parameters.getRawParameterValue("postGain");
    gain.setGain(postGain);
    gain.process(buffer);
}

//==============================================================================
bool GuitarAmpAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* GuitarAmpAudioProcessor::createEditor()
{
    return new GuitarAmpAudioProcessorEditor (*this, parameters);
}

//==============================================================================
void GuitarAmpAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void GuitarAmpAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GuitarAmpAudioProcessor();
}
