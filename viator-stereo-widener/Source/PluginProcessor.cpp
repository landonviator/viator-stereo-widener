#include "PluginProcessor.h"
#include "PluginEditor.h"

ViatorstereowidenerAudioProcessor::ViatorstereowidenerAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                       )
#endif
{
}

ViatorstereowidenerAudioProcessor::~ViatorstereowidenerAudioProcessor()
{
}

const juce::String ViatorstereowidenerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ViatorstereowidenerAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ViatorstereowidenerAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ViatorstereowidenerAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ViatorstereowidenerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ViatorstereowidenerAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ViatorstereowidenerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ViatorstereowidenerAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String ViatorstereowidenerAudioProcessor::getProgramName (int index)
{
    return {};
}

void ViatorstereowidenerAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

#pragma mark Prepare
void ViatorstereowidenerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    m_spec.sampleRate = sampleRate;
    m_spec.maximumBlockSize = samplesPerBlock;
    m_spec.numChannels = getTotalNumInputChannels();
    
    for (int i = 0; i < 100; i++)
    {
        _filters.add(std::make_unique<viator_dsp::SVFilter<float>>());
        _filters[i]->prepare(m_spec);
        _filters[i]->setParameter(viator_dsp::SVFilter<float>::ParameterId::kType, viator_dsp::SVFilter<float>::FilterType::kLowPass);
        _filters[i]->setParameter(viator_dsp::SVFilter<float>::ParameterId::kQType, viator_dsp::SVFilter<float>::QType::kParametric);
        _filters[i]->setParameter(viator_dsp::SVFilter<float>::ParameterId::kCutoff, 1000.0);
        _filters[i]->setParameter(viator_dsp::SVFilter<float>::ParameterId::kQ, 0.1);
        _filters[i]->setParameter(viator_dsp::SVFilter<float>::ParameterId::kGain, 0.0);
    }
    
    _cpuMeasureModule.reset(sampleRate, samplesPerBlock);
    _cpuLoad.store(0.0);
}

void ViatorstereowidenerAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ViatorstereowidenerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

#pragma mark Process
void ViatorstereowidenerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::AudioProcessLoadMeasurer::ScopedTimer s(_cpuMeasureModule);
    
    juce::dsp::AudioBlock<float> block {buffer};
    
    for (auto& filter : _filters)
    {
        filter->process(juce::dsp::ProcessContextReplacing<float>(block));
    }
    
    _cpuLoad.store(_cpuMeasureModule.getLoadAsPercentage());
}

float ViatorstereowidenerAudioProcessor::getCPULoad()
{
    return _cpuLoad.load();
}

bool ViatorstereowidenerAudioProcessor::hasEditor() const{return true;}

#pragma mark Editor
juce::AudioProcessorEditor* ViatorstereowidenerAudioProcessor::createEditor()
{
    return new ViatorstereowidenerAudioProcessorEditor (*this);
}

#pragma mark Get/Set State
void ViatorstereowidenerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void ViatorstereowidenerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter(){return new ViatorstereowidenerAudioProcessor();}
