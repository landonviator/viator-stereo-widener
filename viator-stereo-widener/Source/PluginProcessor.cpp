#include "PluginProcessor.h"
#include "PluginEditor.h"

ViatorstereowidenerAudioProcessor::ViatorstereowidenerAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                       )
, _treeState(*this, nullptr, "PARAMETERS", createParameterLayout())
#endif
{
    // sliders
    for (int i = 0; i < _parameterMap.getSliderParams().size(); i++)
    {
        _treeState.addParameterListener(_parameterMap.getSliderParams()[i].paramID, this);
    }
    
    // buttons
    for (int i = 0; i < _parameterMap.getButtonParams().size(); i++)
    {
        _treeState.addParameterListener(_parameterMap.getButtonParams()[i]._id, this);
    }
}

ViatorstereowidenerAudioProcessor::~ViatorstereowidenerAudioProcessor()
{
    // sliders
    for (int i = 0; i < _parameterMap.getSliderParams().size(); i++)
    {
        _treeState.removeParameterListener(_parameterMap.getSliderParams()[i].paramID, this);
    }
    
    // buttons
    for (int i = 0; i < _parameterMap.getButtonParams().size(); i++)
    {
        _treeState.removeParameterListener(_parameterMap.getButtonParams()[i]._id, this);
    }
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

juce::AudioProcessorValueTreeState::ParameterLayout ViatorstereowidenerAudioProcessor::createParameterLayout()
{
    std::vector <std::unique_ptr<juce::RangedAudioParameter>> params;
    
    // sliders
    for (int i = 0; i < _parameterMap.getSliderParams().size(); i++)
    {
        auto param = _parameterMap.getSliderParams()[i];

        if (param.isInt == ViatorParameters::SliderParameterData::NumericType::kInt || param.isSkew == ViatorParameters::SliderParameterData::SkewType::kSkew)
        {
            auto range = juce::NormalisableRange<float>(param.min, param.max);

            if (param.isSkew == ViatorParameters::SliderParameterData::SkewType::kSkew)
            {
                range.setSkewForCentre(param.center);
            }

            params.push_back (std::make_unique<juce::AudioProcessorValueTreeState::Parameter>(juce::ParameterID { param.paramID, 1 }, param.name, param.name, range, param.initial, valueToTextFunction, textToValueFunction));
        }

        else
        {
            params.push_back (std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { param.paramID, 1 }, param.name, param.min, param.max, param.initial));
        }
    }
    
    // buttons
    for (int i = 0; i < _parameterMap.getButtonParams().size(); i++)
    {
        auto param = _parameterMap.getButtonParams()[i];
        params.push_back (std::make_unique<juce::AudioParameterBool>(juce::ParameterID { param._id, 1 }, param._name, false));
    }
    
    return { params.begin(), params.end() };
}

void ViatorstereowidenerAudioProcessor::parameterChanged(const juce::String &parameterID, float newValue)

{
    updateParameters();
}

void ViatorstereowidenerAudioProcessor::updateParameters()
{
}

#pragma mark Prepare
void ViatorstereowidenerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    m_spec.sampleRate = sampleRate;
    m_spec.maximumBlockSize = samplesPerBlock;
    m_spec.numChannels = getTotalNumInputChannels();
    
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
    
    auto width = _treeState.getRawParameterValue(ViatorParameters::widthID)->load();
    
    if (buffer.getNumChannels() > 1)
    {
        for (int channel = 0; channel < buffer.getNumChannels(); channel++)
        {
            auto* data = buffer.getWritePointer(channel);
            auto* leftInputData = buffer.getWritePointer(0);
            auto* rightInputData = buffer.getWritePointer(1);
            for (int sample = 0; sample < buffer.getNumSamples(); sample++)
            {
                auto mid_x = leftInputData[sample] + rightInputData[sample];
                auto side_x = leftInputData[sample] - rightInputData[sample];
                
                auto newMid = juce::jmap(width, 0.0f, 1.0f, 0.5f, 0.25f) * mid_x;
                auto newSides = juce::jmap(width, 0.0f, 1.0f, 0.5f, 0.75f) * side_x;
                auto newLeftOut = newMid + newSides;
                auto newRightOut = newMid - newSides;
                
                leftInputData[sample] = newLeftOut;
                rightInputData[sample] = newRightOut;
            }
        }
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
    //return new ViatorstereowidenerAudioProcessorEditor (*this);
    return new juce::GenericAudioProcessorEditor (*this);
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
