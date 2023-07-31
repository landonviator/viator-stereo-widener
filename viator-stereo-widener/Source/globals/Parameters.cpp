#include "Parameters.h"


ViatorParameters::Params::Params()
{
    initSliderParams();
    initButtonParams();
}

void ViatorParameters::Params::initSliderParams()
{
    using skew = SliderParameterData::SkewType;
    using type = SliderParameterData::NumericType;
    
    // input
    _sliderParams.push_back({ViatorParameters::inputID, ViatorParameters::inputName, -20.0f, 20.0f, 0.0f, skew::kNoSkew, 0.0, type::kFloat});
    
    // output
    _sliderParams.push_back({ViatorParameters::outputID, ViatorParameters::outputName, -20.0f, 20.0f, 0.0f, skew::kNoSkew, 0.0, type::kFloat});
    
    _sliderParams.push_back({ViatorParameters::widthID, ViatorParameters::widthName, 0.0f, 1.0f, 0.0f, skew::kNoSkew, 0.0, type::kFloat});
}

void ViatorParameters::Params::initButtonParams()
{
    _buttonParams.push_back({ViatorParameters::rangeID, ViatorParameters::rangeName, false});
}
