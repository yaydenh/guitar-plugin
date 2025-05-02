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
    parameters(*this, nullptr, "parameters", createParameterLayout())
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
    toneStack.prepare(spec);
    eq.prepare(spec);
    gain.prepare(spec);
    noiseGate.prepare(spec);
    cabSim.prepare(spec);
    compressor.prepare(spec);
    reverb.prepare(spec);
    overdrive.prepare(spec);
    distortion.prepare(spec);
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

    // distortion pedal
    bool distortionOn = parameters.getParameterAsValue("distortionOn").getValue();
    float distortionDrive = *parameters.getRawParameterValue("distortionDrive");
    float distortionTone = *parameters.getRawParameterValue("distortionTone");
    float distortionLevel = *parameters.getRawParameterValue("distortionLevel");
    distortion.configure(distortionDrive, distortionTone, distortionLevel);
    if (distortionOn) distortion.process(buffer);

    // overdrive pedal
    bool overdriveOn = parameters.getParameterAsValue("overdriveOn").getValue();
    float overdriveDrive = *parameters.getRawParameterValue("overdriveDrive");
    float overdriveTone = *parameters.getRawParameterValue("overdriveTone");
    float overdriveLevel = *parameters.getRawParameterValue("overdriveLevel");
    overdrive.configure(overdriveDrive, overdriveTone, overdriveLevel);
    if (overdriveOn) overdrive.process(buffer);

    // preamp
    preamp.test = test;
    int preampMode = parameters.getParameterAsValue("preampMode").getValue();
    preamp.setMode(static_cast<PreampProcessor::Mode>(preampMode));
    float preampGain = *parameters.getRawParameterValue("preGain");
    preamp.setGain(preampGain);
    preamp.process(buffer);

    // tone stack
    float toneStackBass = *parameters.getRawParameterValue("toneStackBass");
    float toneStackMid = *parameters.getRawParameterValue("toneStackMid");
    float toneStackTreble = *parameters.getRawParameterValue("toneStackTreble");
    toneStack.updateEQ(toneStackBass, toneStackMid, toneStackTreble);
    toneStack.process(buffer);

    // cab sim
    cabSim.process(buffer);

    // post EQ
    float postBass = *parameters.getRawParameterValue("postBassEQ");
    float postMid = *parameters.getRawParameterValue("postMidEQ");
    float postTreble = *parameters.getRawParameterValue("postTrebleEQ");
    eq.updateEQ(postBass, postMid, postTreble);
    eq.process(buffer);

    // compressor pedal
    bool compressorOn = parameters.getParameterAsValue("compressorOn").getValue();
    float compressorThreshold = *parameters.getRawParameterValue("compressorThreshold");
    float compressorAttack = *parameters.getRawParameterValue("compressorAttack");
    float compressorRelease = *parameters.getRawParameterValue("compressorRelease");
    float compressorKneeWidth = *parameters.getRawParameterValue("compressorKneeWidth");
    float compressorRatio = *parameters.getRawParameterValue("compressorRatio");
    float compressorMakeUpGain = *parameters.getRawParameterValue("compressorMakeUpGain");
    float compressorBlend = *parameters.getRawParameterValue("compressorBlend");
    compressor.configure(compressorThreshold,
                         compressorAttack,
                         compressorRelease,
                         compressorKneeWidth,
                         compressorRatio,
                         compressorMakeUpGain,
                         compressorBlend);
    if (compressorOn) compressor.process(buffer);

    // reverb pedal
    bool reverbOn = parameters.getParameterAsValue("reverbOn").getValue();
    float reverbPreDelay = *parameters.getRawParameterValue("reverbPreDelay");
    float reverbHighCutFreq = *parameters.getRawParameterValue("reverbHighCutFreq");
    float reverbDiffusion = *parameters.getRawParameterValue("reverbDiffusion");
    float reverbHighFreqDamping = *parameters.getRawParameterValue("reverbHighFreqDamping");
    float reverbDecayFactor = *parameters.getRawParameterValue("reverbDecayFactor");
    float reverbWetDryMix = *parameters.getRawParameterValue("reverbWetDryMix");
    reverb.configure(reverbPreDelay,
        reverbHighCutFreq,
        reverbDiffusion,
        reverbHighFreqDamping,
        reverbDecayFactor,
        reverbWetDryMix);
    if (reverbOn) reverb.process(buffer);

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

juce::AudioProcessorValueTreeState::ParameterLayout GuitarAmpAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout params;
    // parameter for testing
    params.add(std::make_unique<juce::AudioParameterBool>("test", "test", false));

    // preamp parameters
    params.add(std::make_unique<juce::AudioParameterChoice>("preampMode", "preampMode",
        juce::StringArray{ "Clean", "Crunch", "Lead" }, 0
    ));
    params.add(std::make_unique<juce::AudioParameterFloat>("preGain", "preGain", -24.0f, 24.0f, 0.0f));

    // tone stack parameteres
    params.add(std::make_unique<juce::AudioParameterFloat>("toneStackBass", "toneStackBass", -12.0f, 12.0f, 0.0f));
    params.add(std::make_unique<juce::AudioParameterFloat>("toneStackMid", "toneStackMid", -12.0f, 12.0f, 0.0f));
    params.add(std::make_unique<juce::AudioParameterFloat>("toneStackTreble", "toneStackTreble", -12.0f, 12.0f, 0.0f));

    // eq parameters
    params.add(std::make_unique<juce::AudioParameterFloat>("postBassEQ", "postBassEQ", -12.0f, 12.0f, 0.0f));
    params.add(std::make_unique<juce::AudioParameterFloat>("postMidEQ", "postMidEQ", -12.0f, 12.0f, 0.0f));
    params.add(std::make_unique<juce::AudioParameterFloat>("postTrebleEQ", "postTrebleEQ", -12.0f, 12.0f, 0.0f));

    // output gain
    params.add(std::make_unique<juce::AudioParameterFloat>("postGain", "postGain", -24.0f, 24.0f, 0.0f));

    // noise gate
    params.add(std::make_unique<juce::AudioParameterFloat>("noiseGateThreshold", "noiseGateThreshold", -96.0f, 0.0f, -96.0f));

    // compressor pedal
    params.add(std::make_unique<juce::AudioParameterBool>("compressorOn", "compressorOn", false));
    params.add(std::make_unique<juce::AudioParameterFloat>("compressorLevel", "compressorLevel", 0.0f, 5.0f, 0.0f));
    params.add(std::make_unique<juce::AudioParameterFloat>("compressorSustain", "compressorSustain", 0.0f, 5.0f, 0.0f));
    params.add(std::make_unique<juce::AudioParameterFloat>("compressorBlend", "compressorBlend", 0.0f, 1.0f, 1.0f));
    params.add(std::make_unique<juce::AudioParameterFloat>("compressorThreshold", "compressorThreshold", -50.0f, 0.0f, -20.0f));
    params.add(std::make_unique<juce::AudioParameterFloat>("compressorAttack", "compressorAttack", 0.5f, 50.0f, 10.0f));
    params.add(std::make_unique<juce::AudioParameterFloat>("compressorRelease", "compressorRelease", 50.0f, 500.0f, 150.0f));
    params.add(std::make_unique<juce::AudioParameterFloat>("compressorKneeWidth", "compressorKneeWidth", 0.0f, 20.0f, 6.0f));
    params.add(std::make_unique<juce::AudioParameterFloat>("compressorRatio", "compressorRatio", 1.0f, 50.0f, 2.0f));
    params.add(std::make_unique<juce::AudioParameterFloat>("compressorMakeUpGain", "compressorMakeUpGain", -10.0f, 20.0f, 0.0f));

    // reverb pedal
    params.add(std::make_unique<juce::AudioParameterBool>("reverbOn", "reverbOn", false));
    params.add(std::make_unique<juce::AudioParameterFloat>("reverbPreDelay", "reverbPreDelay", 0.0f, 1.0f, 0.0f));
    params.add(std::make_unique<juce::AudioParameterFloat>("reverbHighCutFreq", "reverbHighCutFreq", 20.0f, 20000.0f, 20000.0f));
    params.add(std::make_unique<juce::AudioParameterFloat>("reverbDiffusion", "reverbDiffusion", 0.0f, 1.0f, 0.5f));
    params.add(std::make_unique<juce::AudioParameterFloat>("reverbHighFreqDamping", "reverbHighFreqDamping", 0.0f, 1.0f, 0.5f));
    params.add(std::make_unique<juce::AudioParameterFloat>("reverbDecayFactor", "reverbHighFreqDamping", 0.0f, 1.0f, 0.5f));
    params.add(std::make_unique<juce::AudioParameterFloat>("reverbWetDryMix", "reverbWetDryMix", 0.0f, 1.0f, 0.3f));

    // overdrive pedal
    params.add(std::make_unique<juce::AudioParameterBool>("overdriveOn", "overdriveOn", false));
    params.add(std::make_unique<juce::AudioParameterFloat>("overdriveDrive", "overdriveDrive", 0.0f, 1.0f, 0.5f));
    params.add(std::make_unique<juce::AudioParameterFloat>("overdriveTone", "overdriveTone", 0.0f, 1.0f, 0.5f));
    params.add(std::make_unique<juce::AudioParameterFloat>("overdriveLevel", "overdriveLevel", -12.0f, 12.0f, 0.0f));

    // overdrive pedal
    params.add(std::make_unique<juce::AudioParameterBool>("overdriveOn", "overdriveOn", false));
    params.add(std::make_unique<juce::AudioParameterFloat>("overdriveDrive", "overdriveDrive", 0.0f, 1.0f, 0.5f));
    params.add(std::make_unique<juce::AudioParameterFloat>("overdriveTone", "overdriveTone", 0.0f, 1.0f, 0.5f));
    params.add(std::make_unique<juce::AudioParameterFloat>("overdriveLevel", "overdriveLevel", -12.0f, 12.0f, 0.0f));

    // distortion pedal
    params.add(std::make_unique<juce::AudioParameterBool>("distortionOn", "distortionOn", false));
    params.add(std::make_unique<juce::AudioParameterFloat>("distortionDrive", "distortionDrive", 0.0f, 1.0f, 0.5f));
    params.add(std::make_unique<juce::AudioParameterFloat>("distortionTone", "distortionTone", 0.0f, 1.0f, 0.5f));
    params.add(std::make_unique<juce::AudioParameterFloat>("distortionLevel", "distortionLevel", -12.0f, 12.0f, 0.0f));

    // fuzz pedal
    params.add(std::make_unique<juce::AudioParameterBool>("fuzzOn", "fuzzOn", false));
    params.add(std::make_unique<juce::AudioParameterFloat>("fuzzSustain", "fuzzSustain", 0.0f, 1.0f, 0.5f));
    params.add(std::make_unique<juce::AudioParameterFloat>("fuzzTone", "fuzzTone", 0.0f, 1.0f, 0.5f));
    params.add(std::make_unique<juce::AudioParameterFloat>("fuzzLevel", "fuzzLevel", -12.0f, 12.0f, 0.0f));

    return params;
}