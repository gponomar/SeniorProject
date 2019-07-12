//-----------------------------------------------------------------------------
// Project     : VST SDK
//
// Category    : Examples
// Filename    : public.sdk/samples/vst/note_expression_synth/source/note_expression_synth_voice.h
// Created by  : Steinberg, 02/2010
// Description : 
//
//-----------------------------------------------------------------------------
// LICENSE
// (c) 2019, Steinberg Media Technologies GmbH, All Rights Reserved
//-----------------------------------------------------------------------------
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
// 
//   * Redistributions of source code must retain the above copyright notice, 
//     this list of conditions and the following disclaimer.
//   * Redistributions in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation 
//     and/or other materials provided with the distribution.
//   * Neither the name of the Steinberg Media Technologies nor the names of its
//     contributors may be used to endorse or promote products derived from this 
//     software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
// IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
// OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

#pragma once

#include "../../common/voicebase.h"
#include "../../common/logscale.h"
#include "brownnoise.h"
#include "filter.h"
#include "note_expression_synth_controller.h"
#include "pluginterfaces/vst/ivstevents.h"
#include "pluginterfaces/base/futils.h"
#include <cmath>
#include <algorithm>

#ifndef M_PI
#define M_PI			3.14159265358979323846
#endif
#ifndef M_PI_MUL_2
#define M_PI_MUL_2		6.28318530717958647692
#endif

#define MAX_VOLUME .8
namespace Steinberg {
namespace Vst {
namespace NoteExpressionSynth {

//-----------------------------------------------------------------------------
struct GlobalParameterState
{
	BrownNoise<float>* noiseBuffer;
    BrownNoise<float>* noiseBufferTwo;

	ParamValue masterVolume;	// [0, +1]
	ParamValue masterTuning;	// [-1, +1]
	ParamValue velToLevel;		// [0, +1]

	ParamValue noiseVolume;		// [0, +1]
	ParamValue sinusVolume;		// [0, +1]
	ParamValue triangleVolume;	// [0, +1]
	ParamValue squareVolume;	// [0, +1]
    
    ParamValue noiseVolumeTwo;        // [0, +1]
    ParamValue sinusVolumeTwo;        // [0, +1]
    ParamValue triangleVolumeTwo;    // [0, +1]
    ParamValue squareVolumeTwo;    // [0, +1]
	
	ParamValue attackTime;		// [0, +1]
	ParamValue releaseTime;		// [0, +1]
    
    
    ParamValue sustainVolume;        // [0, +1]
    ParamValue decayTime;        // [0, +1]

	ParamValue sinusDetune;		// [-1, +1]
	ParamValue triangleSlop;	// [0, +1]
    
    ParamValue sinusDetuneTwo;        // [-1, +1]
    ParamValue triangleSlopTwo;    // [0, +1]

    ParamValue genFreqOne;        // [-1, +1]
    ParamValue genFreqTwo;        // [-1, +1]
    
	ParamValue filterFreq;		// [-1, +1]
	ParamValue filterQ;			// [-1, +1]
	ParamValue freqModDepth;	// [-1, +1]
    
    ParamValue filterOneFreq;        // [-1, +1]
    ParamValue filterOneQ;            // [-1, +1]
    ParamValue freqOneModDepth;    // [-1, +1]
    
    ParamValue filterTwoFreq;        // [-1, +1]
    ParamValue filterTwoQ;            // [-1, +1]
    ParamValue freqTwoModDepth;    // [-1, +1]
	
	int8 filterType;			// [0, 1, 2]
    int8 oscType;            // [0, 1, 2, 3]
    int8 oscTypeTwo;            // [0, 1, 2, 3]
    int8 filterOneType;            // [0, 1, 2]
    int8 filterTwoType;            // [0, 1, 2]
	int8 tuningRange;			// [0, 1]
	
	int8 bypassSNA;				// [0, 1]

	tresult setState (IBStream* stream);
	tresult getState (IBStream* stream);
};

//-----------------------------------------------------------------------------
enum VoiceParameters
{
	kVolumeMod,
	kTuningMod,
	kPanningLeft,
	kPanningRight,
	kNoiseVolume,
    kNoiseVolumeTwo,
	kFilterFrequencyMod,
	kFilterQMod,
    
    kFilterOneFrequencyMod,
    kFilterOneQMod,
    kFilterTwoFrequencyMod,
    kFilterTwoQMod,
    
	kSinusVolume,
	kTriangleVolume,
    kSinusVolumeTwo,
    kTriangleVolumeTwo,
	kFilterType,
    
    kFilterOneType,
    kFilterTwoType,
    
	kTriangleSlope,
	kSinusDetune,
    kTriangleSlopeTwo,
    kSinusDetuneTwo,
	kReleaseTimeMod,
	kAttackTimeMod,
    kSustainVolumeMod,
    kDecayTimeMod,
	kSquareVolume,
    kSquareVolumeTwo,
    kGenFreqOne,
    kGenFreqTwo,

	kNumParameters
};

//-----------------------------------------------------------------------------
class VoiceStatics
{
public:
	//------------------------------------------------------------------------
	static double normalizedLevel2Gain (float normalized)
	{
		double level;
		if (normalized >= 0.5)
			level = scaleHeadRoom * ::pow (10, (normalized - 0.5f) * 24 / 20.0f);
		else
			level = scaleNorm2GainC1 * ::pow (normalized, scaleNorm2GainC2);	

		return level;
	}	

	enum {
		kNumFrequencies = 128
	};

	static float freqTab[kNumFrequencies];
	static const float scaleHeadRoom;
	static const float scaleNorm2GainC1;
	static const float scaleNorm2GainC2;
	static LogScale<ParamValue> freqLogScale;
	static const double kNormTuningOneOctave;
	static const double kNormTuningOneTune;

};

//-----------------------------------------------------------------------------
/** Example Note Expression Synth Voice Class

\sa Steinberg::Vst::VoiceBase
*/
template<class SamplePrecision>
class Voice : public VoiceBase<kNumParameters, SamplePrecision, 2, GlobalParameterState>
{
public:
	Voice ();
	~Voice ();

	void setSampleRate (ParamValue sampleRate) SMTG_OVERRIDE;
	void noteOn (int32 pitch, ParamValue velocity, float tuning, int32 sampleOffset, int32 nId) SMTG_OVERRIDE;
	void noteOff (ParamValue velocity, int32 sampleOffset) SMTG_OVERRIDE;
	bool process (SamplePrecision* outputBuffers[2], int32 numSamples);
	void reset () SMTG_OVERRIDE;

	void setNoteExpressionValue (int32 index, ParamValue value) SMTG_OVERRIDE;

protected:
	uint32 n;
	int32 noisePos;
    int32 noisePosTwo;
	int32 noiseStep;
    int32 noiseStepTwo;
    bool decayStart = false;

	Filter* filter;
    Filter* filterOne;
    Filter* filterTwo;

	SamplePrecision trianglePhase;
	SamplePrecision sinusPhase;
	ParamValue currentTriangleF;
	ParamValue currentSinusF;
    SamplePrecision trianglePhaseTwo;
    SamplePrecision sinusPhaseTwo;
    ParamValue currentTriangleFTwo;
    ParamValue currentSinusFTwo;
	ParamValue currentVolume;
	ParamValue currentPanningLeft;
	ParamValue currentPanningRight;
	ParamValue currentNoiseVolume;
	ParamValue currentSinusVolume;
	ParamValue currentSinusDetune;
	ParamValue currentSquareVolume;
    ParamValue currentNoiseVolumeTwo;
    ParamValue currentSinusVolumeTwo;
    ParamValue currentSinusDetuneTwo;
    ParamValue currentSquareVolumeTwo;
    ParamValue currentSustainVolume;
	ParamValue currentTriangleVolume;
	ParamValue currentTriangleSlope;
    ParamValue currentTriangleVolumeTwo;
    ParamValue currentTriangleSlopeTwo;
	ParamValue currentLPFreq;
	ParamValue currentLPQ;
    ParamValue currentGenFreqOne;
    ParamValue currentGenFreqTwo;
    
    ParamValue currentLPOneFreq;
    ParamValue currentLPOneQ;
    ParamValue currentLPTwoFreq;
    ParamValue currentLPTwoQ;

	ParamValue levelFromVel;
	ParamValue noteOnVolumeRamp;
    ParamValue noteOnVolumeRampDecay;
	ParamValue noteOffVolumeRamp;
};

//-----------------------------------------------------------------------------
template<class SamplePrecision>
void Voice<SamplePrecision>::setNoteExpressionValue (int32 index, ParamValue value)
{
	if (this->globalParameters->bypassSNA)
		return;

	switch (index)
	{
		//------------------------------
		case Steinberg::Vst::kVolumeTypeID:
		{
			ParamValue vol = VoiceStatics::normalizedLevel2Gain ((float)value);
			VoiceBase<kNumParameters, SamplePrecision, 2, GlobalParameterState>::setNoteExpressionValue (kVolumeMod, vol);
			break;
		}
		//------------------------------IMPORTANT STUFFFS 
		case Steinberg::Vst::kTuningTypeID:
		{
			if (value == 0.5)
			{
				VoiceBase<kNumParameters, SamplePrecision, 2, GlobalParameterState>::setNoteExpressionValue (kTuningMod, 0);
			}
			else if (value > 0.5)
			{
				if (this->globalParameters->tuningRange > 0.5)
					value = std::min<ParamValue> (0.5 + 2 * VoiceStatics::kNormTuningOneTune, value);
				else
					value = std::min<ParamValue> (0.5 + VoiceStatics::kNormTuningOneOctave, value);
				VoiceBase<kNumParameters, SamplePrecision, 2, GlobalParameterState>::setNoteExpressionValue (kTuningMod, (value - 0.5) * 2.);
			}
			else
			{
				if (this->globalParameters->tuningRange > 0.5)
					value = std::max<ParamValue> (0.5 - 3 * VoiceStatics::kNormTuningOneTune, value);
				else
					value = std::max<ParamValue> (0.5 - VoiceStatics::kNormTuningOneOctave, value);
				VoiceBase<kNumParameters, SamplePrecision, 2, GlobalParameterState>::setNoteExpressionValue (kTuningMod, (value - 0.5) * 2.);
			}
			break;
		}
            
            
            
            
            
            
            
		//------------------------------
		case Steinberg::Vst::kPanTypeID:
		{
			if (value == 0.5)
			{
				VoiceBase<kNumParameters, SamplePrecision, 2, GlobalParameterState>::setNoteExpressionValue (kPanningLeft, 1.);
				VoiceBase<kNumParameters, SamplePrecision, 2, GlobalParameterState>::setNoteExpressionValue (kPanningRight, 1.);
			}
			else if (value > 0.5)
			{
				VoiceBase<kNumParameters, SamplePrecision, 2, GlobalParameterState>::setNoteExpressionValue (kPanningLeft, 1.);
				VoiceBase<kNumParameters, SamplePrecision, 2, GlobalParameterState>::setNoteExpressionValue (kPanningRight, 1. + ((0.5 - value) * 2.));
			}
			else
			{
				VoiceBase<kNumParameters, SamplePrecision, 2, GlobalParameterState>::setNoteExpressionValue (kPanningLeft, value * 2.);
				VoiceBase<kNumParameters, SamplePrecision, 2, GlobalParameterState>::setNoteExpressionValue (kPanningRight, 1.);
			}
			break;
		}
		//------------------------------
		case Controller::kNoiseVolumeTypeID:
		{
			VoiceBase<kNumParameters, SamplePrecision, 2, GlobalParameterState>::setNoteExpressionValue (kNoiseVolume, value * 2.);
			break;
		}
        //------------------------------
        case Controller::kNoiseVolumeTwoTypeID:
        {
            VoiceBase<kNumParameters, SamplePrecision, 2, GlobalParameterState>::setNoteExpressionValue (kNoiseVolumeTwo, value * 2.);
            break;
        }
		//------------------------------
		case Controller::kSinusVolumeTypeID:
		{
			VoiceBase<kNumParameters, SamplePrecision, 2, GlobalParameterState>::setNoteExpressionValue (kSinusVolume, value * 2.);
			break;
		}
        //------------------------------
        case Controller::kSinusVolumeTwoTypeID:
        {
            VoiceBase<kNumParameters, SamplePrecision, 2, GlobalParameterState>::setNoteExpressionValue (kSinusVolumeTwo, value * 2.);
            break;
        }
		//------------------------------
		case Controller::kTriangleVolumeTypeID:
		{
			VoiceBase<kNumParameters, SamplePrecision, 2, GlobalParameterState>::setNoteExpressionValue (kTriangleVolume, value * 2.);
			break;
		}
        //------------------------------
        case Controller::kTriangleVolumeTwoTypeID:
        {
            VoiceBase<kNumParameters, SamplePrecision, 2, GlobalParameterState>::setNoteExpressionValue (kTriangleVolumeTwo, value * 2.);
            break;
        }
		//------------------------------
		case Controller::kSquareVolumeTypeID:
		{
			VoiceBase<kNumParameters, SamplePrecision, 2, GlobalParameterState>::setNoteExpressionValue (kSquareVolume, value * 2.);
			break;
		}
        //------------------------------
        case Controller::kSquareVolumeTwoTypeID:
        {
            VoiceBase<kNumParameters, SamplePrecision, 2, GlobalParameterState>::setNoteExpressionValue (kSquareVolumeTwo, value * 2.);
            break;
        }
		//------------------------------
		case Controller::kFilterFreqModTypeID:
		{
			VoiceBase<kNumParameters, SamplePrecision, 2, GlobalParameterState>::setNoteExpressionValue (kFilterFrequencyMod, (value - 0.5) * 2.);
			break;
		}
		//------------------------------
		case Controller::kFilterQModTypeID:
		{
			VoiceBase<kNumParameters, SamplePrecision, 2, GlobalParameterState>::setNoteExpressionValue (kFilterQMod, (value - 0.5) * 2.);
			break;
		}
		//------------------------------
		case Controller::kFilterTypeTypeID:
		{
			filter->setType ((Filter::Type)std::min<int32> ((int32)(NUM_FILTER_TYPE * value), NUM_FILTER_TYPE - 1));
			break;
		}
            
        case Controller::kFilterOneFreqModTypeID:
        {
            VoiceBase<kNumParameters, SamplePrecision, 2, GlobalParameterState>::setNoteExpressionValue (kFilterOneFrequencyMod, (value - 0.5) * 2.);
            break;
        }
            //------------------------------
        case Controller::kFilterOneQModTypeID:
        {
            VoiceBase<kNumParameters, SamplePrecision, 2, GlobalParameterState>::setNoteExpressionValue (kFilterOneQMod, (value - 0.5) * 2.);
            break;
        }
            //------------------------------
        case Controller::kFilterOneTypeTypeID:
        {
            filterOne->setType ((Filter::Type)std::min<int32> ((int32)(NUM_FILTER_TYPE * value), NUM_FILTER_TYPE - 1));
            break;
        }
        case Controller::kGenFreqOneTypeID:
        {
            VoiceBase<kNumParameters, SamplePrecision, 2, GlobalParameterState>::setNoteExpressionValue (kGenFreqOne, (value - 0.5) * 2.);
            break;
        }
        case Controller::kGenFreqTwoTypeID:
        {
            VoiceBase<kNumParameters, SamplePrecision, 2, GlobalParameterState>::setNoteExpressionValue (kGenFreqTwo, (value - 0.5) * 2.);
            break;
            
        }
            
            
        case Controller::kFilterTwoFreqModTypeID:
        {
            VoiceBase<kNumParameters, SamplePrecision, 2, GlobalParameterState>::setNoteExpressionValue (kFilterTwoFrequencyMod, (value - 0.5) * 2.);
            break;
        }
            //------------------------------
        case Controller::kFilterTwoQModTypeID:
        {
            VoiceBase<kNumParameters, SamplePrecision, 2, GlobalParameterState>::setNoteExpressionValue (kFilterTwoQMod, (value - 0.5) * 2.);
            break;
        }
            //------------------------------
        case Controller::kFilterTwoTypeTypeID:
        {
            filterTwo->setType ((Filter::Type)std::min<int32> ((int32)(NUM_FILTER_TYPE * value), NUM_FILTER_TYPE - 1));
            break;
        }
		//------------------------------
		case Controller::kTriangleSlopeTypeID:
		{
			VoiceBase<kNumParameters, SamplePrecision, 2, GlobalParameterState>::setNoteExpressionValue (kTriangleSlope, value);
			break;
		}
        //------------------------------
        case Controller::kTriangleSlopeTwoTypeID:
        {
            VoiceBase<kNumParameters, SamplePrecision, 2, GlobalParameterState>::setNoteExpressionValue (kTriangleSlopeTwo, value);
            break;
        }
		//------------------------------
		case Controller::kSinusDetuneTypeID:
		{
			VoiceBase<kNumParameters, SamplePrecision, 2, GlobalParameterState>::setNoteExpressionValue (kSinusDetune, value);
			break;
		}
        //------------------------------
        case Controller::kSinusDetuneTwoTypeID:
        {
            VoiceBase<kNumParameters, SamplePrecision, 2, GlobalParameterState>::setNoteExpressionValue (kSinusDetuneTwo, value);
            break;
        }
		//------------------------------
		case Controller::kReleaseTimeModTypeID:
		{
			VoiceBase<kNumParameters, SamplePrecision, 2, GlobalParameterState>::setNoteExpressionValue (kReleaseTimeMod, 2 * (value - 0.5));
			break;
		}
		//------------------------------
		case Controller::kAttackTimeModTypeID:
		{
			VoiceBase<kNumParameters, SamplePrecision, 2, GlobalParameterState>::setNoteExpressionValue(kAttackTimeMod, 2 * (value - 0.5));
		}
            
        case Controller::kSustainVolumeModTypeID:
        {
            //VoiceBase<kNumParameters, SamplePrecision, 2, //GlobalParameterState>::setNoteExpressionValue(kSustainVolumeMod, 2 * (value - 0.5));
            ParamValue susVol = VoiceStatics::normalizedLevel2Gain ((float)value);
            VoiceBase<kNumParameters, SamplePrecision, 2, GlobalParameterState>::setNoteExpressionValue (kSustainVolumeMod, susVol);
            break;
        }
            
        case Controller::kDecayTimeModTypeID:
        {
            VoiceBase<kNumParameters, SamplePrecision, 2, GlobalParameterState>::setNoteExpressionValue(kDecayTimeMod, 2 * (value - 0.5));
        }
		//------------------------------
		default:
		{
 			VoiceBase<kNumParameters, SamplePrecision, 2, GlobalParameterState>::setNoteExpressionValue (index, value);
			break;
		}
	}
}

//-----------------------------------------------------------------------------
template<class SamplePrecision>
bool Voice<SamplePrecision>::process (SamplePrecision* outputBuffers[2], int32 numSamples)
{
	//---compute tuning-------------------------

	// main tuning
	ParamValue tuningInHz = 0.;
	if (this->values[kTuningMod] != 0. || this->globalParameters->masterTuning != 0 || this->tuning != 0)
	{
		tuningInHz = VoiceStatics::freqTab[this->pitch] * (::pow (2.0, (this->values[kTuningMod] * 10 + this->globalParameters->masterTuning * 2.0 / 12.0 + this->tuning)) - 1);
	}
	
    //Triangle one
    double freqLogValOne = VoiceStatics::freqLogScale.scale(this->globalParameters->genFreqOne);
    double genFreqOneHz =(VoiceStatics::freqTab[this->pitch] + tuningInHz + freqLogValOne - 261);
    if (genFreqOneHz < 10)
    {
        genFreqOneHz = 10;
    }
	ParamValue triangleFreq = genFreqOneHz * M_PI_MUL_2 / this->getSampleRate () / 2.;
	if (currentTriangleF == -1)
		currentTriangleF = triangleFreq;
	// check for frequency changes and update the phase so that it is crackle free
	if (triangleFreq != currentTriangleF)
	{
		// update phase
		trianglePhase = (SamplePrecision)((currentTriangleF - triangleFreq) * n + trianglePhase);
		currentTriangleF = triangleFreq;
	}
    
    //triangle two
    double freqLogValTwo = VoiceStatics::freqLogScale.scale(this->globalParameters->genFreqTwo);
    double genFreqTwoHz =(VoiceStatics::freqTab[this->pitch] + tuningInHz + freqLogValTwo - 261);
    if (genFreqTwoHz < 10)
    {
        genFreqTwoHz = 10;
    }
    ParamValue triangleFreqTwo = genFreqTwoHz * M_PI_MUL_2 / this->getSampleRate () / 2.;
    if (currentTriangleFTwo == -1)
        currentTriangleFTwo = triangleFreqTwo;
    // check for frequency changes and update the phase so that it is crackle free
    if (triangleFreqTwo != currentTriangleFTwo)
    {
        // update phase
        trianglePhaseTwo = (SamplePrecision)((currentTriangleFTwo - triangleFreqTwo) * n + trianglePhaseTwo);
        currentTriangleFTwo = triangleFreqTwo;
    }

	// Sinus Detune one
	if (currentSinusDetune != this->values[kSinusDetune])
	{
		currentSinusDetune = VoiceStatics::freqTab[this->pitch] * (::pow (2.0, this->values[kSinusDetune] * 2.0 / 12.0) - 1);
	}
    freqLogValOne = VoiceStatics::freqLogScale.scale(this->globalParameters->genFreqOne);
    genFreqOneHz =(VoiceStatics::freqTab[this->pitch] + tuningInHz + currentSinusDetune + freqLogValOne - 261);//middleC
    if (genFreqOneHz < 10)
    {
        genFreqOneHz = 10;
    }
    ParamValue sinusFreq = (genFreqOneHz) * M_PI_MUL_2 / this->getSampleRate ();
    
	if (currentSinusF == -1)
		currentSinusF = sinusFreq;
	if (sinusFreq != currentSinusF)
	{
		// update phase
		sinusPhase = (SamplePrecision)((currentSinusF - sinusFreq) * n) + sinusPhase;
		currentSinusF = sinusFreq;
	}

    // Sinus Detune two
    if (currentSinusDetuneTwo != this->values[kSinusDetuneTwo])
    {
        currentSinusDetuneTwo = VoiceStatics::freqTab[this->pitch] * (::pow (2.0, this->values[kSinusDetuneTwo] * 2.0 / 12.0) - 1);
    }
    freqLogValTwo = VoiceStatics::freqLogScale.scale(this->globalParameters->genFreqTwo);
    genFreqTwoHz = (VoiceStatics::freqTab[this->pitch] + tuningInHz + currentSinusDetuneTwo +  freqLogValTwo - 261);//middleC
    if (genFreqTwoHz < 10)
    {
        genFreqTwoHz = 10;
    }
    ParamValue sinusFreqTwo = (genFreqTwoHz) * M_PI_MUL_2 / this->getSampleRate ();
    
    if (currentSinusFTwo == -1)
        currentSinusFTwo = sinusFreqTwo;
    if (sinusFreqTwo != currentSinusFTwo)
    {
        // update phase
        sinusPhaseTwo = (SamplePrecision)((currentSinusFTwo - sinusFreqTwo) * n) + sinusPhaseTwo;
        currentSinusFTwo = sinusFreqTwo;
    }
    
	//---calculate parameter ramps
	ParamValue volumeRamp = 0.;
	ParamValue panningLeftRamp = 0.;
	ParamValue panningRightRamp = 0.;
	ParamValue noiseVolumeRamp = 0.;
    ParamValue noiseVolumeRampTwo = 0.;
	ParamValue sinusVolumeRamp = 0.;
    ParamValue sinusVolumeRampTwo = 0.;
	ParamValue triangleVolumeRamp = 0.;
    ParamValue triangleVolumeRampTwo = 0.;
	ParamValue squareVolumeRamp = 0.;
    ParamValue squareVolumeRampTwo = 0.;
	ParamValue filterFreqRamp = 0.;
    ParamValue genFreqRampOne = 0.;
    ParamValue genFreqRampTwo = 0.;
	ParamValue filterQRamp = 0.;
    ParamValue filterOneFreqRamp = 0.;
    ParamValue filterOneQRamp = 0.;
    ParamValue filterTwoFreqRamp = 0.;
    ParamValue filterTwoQRamp = 0.;
	ParamValue triangleSlopeRamp = 0.;
    ParamValue triangleSlopeRampTwo = 0.;
	ParamValue rampTime = std::max<ParamValue> ((ParamValue)numSamples, (this->sampleRate * 0.005));
	
	ParamValue wantedVolume = VoiceStatics::normalizedLevel2Gain ((float)Bound (0.0, 1.0, this->globalParameters->masterVolume * levelFromVel + this->values[kVolumeMod]));
	if (wantedVolume != currentVolume)
	{
		volumeRamp = (wantedVolume - currentVolume) / rampTime;
	}

	if (this->values[kPanningLeft] != currentPanningLeft)
	{
		panningLeftRamp = (this->values[kPanningLeft] - currentPanningLeft) / rampTime;
	}
	if (this->values[kPanningRight] != currentPanningRight)
	{
		panningRightRamp = (this->values[kPanningRight] - currentPanningRight) / rampTime;
	}
	if (this->values[kNoiseVolume] != currentNoiseVolume)
	{
		noiseVolumeRamp = (this->values[kNoiseVolume] - currentNoiseVolume) / rampTime;
	}
    if (this->values[kNoiseVolumeTwo] != currentNoiseVolumeTwo)
    {
        noiseVolumeRampTwo = (this->values[kNoiseVolumeTwo] - currentNoiseVolumeTwo) / rampTime;
    }
	if (this->values[kSinusVolume] != currentSinusVolume)
	{
		sinusVolumeRamp = (this->values[kSinusVolume] - currentSinusVolume) / rampTime;
	}
    if (this->values[kSinusVolumeTwo] != currentSinusVolumeTwo)
    {
        sinusVolumeRampTwo = (this->values[kSinusVolumeTwo] - currentSinusVolumeTwo) / rampTime;
    }
	if (this->values[kSquareVolume] != currentSquareVolume)
	{
		squareVolumeRamp = (this->values[kSquareVolume] - currentSquareVolume) / rampTime;
	}
    if (this->values[kSquareVolumeTwo] != currentSquareVolumeTwo)
    {
        squareVolumeRampTwo = (this->values[kSquareVolumeTwo] - currentSquareVolumeTwo) / rampTime;
    }
	if (this->values[kTriangleVolume] != currentTriangleVolume)
	{
		triangleVolumeRamp = (this->values[kTriangleVolume] - currentTriangleVolume) / rampTime;
	}
    if (this->values[kTriangleVolumeTwo] != currentTriangleVolumeTwo)
    {
        triangleVolumeRampTwo = (this->values[kTriangleVolumeTwo] - currentTriangleVolumeTwo) / rampTime;
    }

    
//    ParamValue wantedGenFreqOne = Bound (0., 1., this->globalParameters->filterFreq + this->globalParameters->freqModDepth * this->values[kFilterFrequencyMod]);
//    if (wantedGenFreqOne != currentGenFreqOne)
//    {
//        genFreqRampOne = (wantedGenFreqOne - currentGenFreqOne) / rampTime;
//    }
//    ParamValue wantedGenFreqTwo = Bound (0., 1., this->globalParameters->filterFreq + this->globalParameters->freqModDepth * this->values[kFilterFrequencyMod]);
//    if (wantedGenFreqTwo != currentGenFreqTwo)
//    {
//        genFreqRampTwo = (wantedGenFreqTwo - currentGenFreqTwo) / rampTime;
//    }
    
	ParamValue wantedLPFreq = Bound (0., 1., this->globalParameters->filterFreq + this->globalParameters->freqModDepth * this->values[kFilterFrequencyMod]);
	if (wantedLPFreq != currentLPFreq)
	{
		filterFreqRamp = (wantedLPFreq - currentLPFreq) / rampTime;
	}
	ParamValue wantedLPQ = Bound (0., 1., this->globalParameters->filterQ + this->values[kFilterQMod]);
	if (wantedLPQ != currentLPQ)
	{
		filterQRamp = (wantedLPQ - currentLPQ) / rampTime;
	}
    //_____________new filters______________________
    ParamValue wantedLPOneFreq = Bound (0., 1., this->globalParameters->filterOneFreq + this->globalParameters->freqOneModDepth * this->values[kFilterOneFrequencyMod]);
    if (wantedLPOneFreq != currentLPOneFreq)
    {
        filterOneFreqRamp = (wantedLPOneFreq - currentLPOneFreq) / rampTime;
    }
    ParamValue wantedLPOneQ = Bound (0., 1., this->globalParameters->filterOneQ + this->values[kFilterOneQMod]);
    if (wantedLPOneQ != currentLPOneQ)
    {
        filterOneQRamp = (wantedLPOneQ - currentLPOneQ) / rampTime;
    }
    ParamValue wantedLPTwoFreq = Bound (0., 1., this->globalParameters->filterTwoFreq + this->globalParameters->freqTwoModDepth * this->values[kFilterTwoFrequencyMod]);
    if (wantedLPTwoFreq != currentLPTwoFreq)
    {
        filterTwoFreqRamp = (wantedLPTwoFreq - currentLPTwoFreq) / rampTime;
    }
    ParamValue wantedLPTwoQ = Bound (0., 1., this->globalParameters->filterTwoQ + this->values[kFilterTwoQMod]);
    if (wantedLPTwoQ != currentLPTwoQ)
    {
        filterTwoQRamp = (wantedLPTwoQ - currentLPTwoQ) / rampTime;
    }
    //____________end new filters ____________
	if (this->values[kTriangleSlope] != currentTriangleSlope)
	{
		triangleSlopeRamp = (this->values[kTriangleSlope] - currentTriangleSlope) / rampTime;
	}
    if (this->values[kTriangleSlopeTwo] != currentTriangleSlopeTwo)
    {
        triangleSlopeRampTwo = (this->values[kTriangleSlopeTwo] - currentTriangleSlopeTwo) / rampTime;
    }

	for (int32 i = 0; i < numSamples; i++)
	{
		this->noteOnSampleOffset--;
		this->noteOffSampleOffset--;

        if (this->noteOffSampleOffset < 0)
        {
            volumeRamp = 0;
            if (decayStart)
            {
                //turn decayStart off in release below
                currentVolume -= noteOnVolumeRampDecay;
                if (currentVolume <= (this->globalParameters->sustainVolume))
                {
                    currentVolume = (this->globalParameters->sustainVolume);
                }
            }
            
            //COME BACK WITH MAX VOLUME .8
            else if (currentVolume < MAX_VOLUME)
            {
                // ramp note on
                currentVolume += noteOnVolumeRamp;
                if (currentVolume > MAX_VOLUME)
                {
                    currentVolume = MAX_VOLUME;
                    decayStart = true;
                }
            }
            else
            {
                this->noteOnSampleOffset = -1;
            }
            
        }
		if (this->noteOnSampleOffset <= 0)
		{
			// we are in Release
			if (this->noteOffSampleOffset == 0)
			{
 				volumeRamp = 0;
				if (currentVolume > 0)
				{
					// ramp note off
					currentVolume -= noteOffVolumeRamp;
					if (currentVolume < 0.)
						currentVolume = 0.;
					this->noteOffSampleOffset++;
				}
				else
				{
					this->noteOffSampleOffset = this->noteOnSampleOffset = -1;
                    decayStart = false;
					return false;
				}
			}
            //STOPPED SECOND GENERATOR HERE
			SamplePrecision sample;
            SamplePrecision sampleTwo;
            
            SamplePrecision oscTwo = (SamplePrecision)sin (n * triangleFreqTwo + trianglePhaseTwo);
            if (this->globalParameters->oscTypeTwo == 0)
            {
                sampleTwo = (SamplePrecision)(sin (n * sinusFreqTwo + sinusPhaseTwo) * currentSinusVolumeTwo);
            }
            else if (this->globalParameters->oscTypeTwo == 1)
            {
                sampleTwo = (SamplePrecision)((::floor (oscTwo) + 0.5) * currentSquareVolumeTwo);
            }
            else if (this->globalParameters->oscTypeTwo == 2)
            {
                sampleTwo = (SamplePrecision)((oscTwo - ::fabs(sin (n * triangleFreqTwo + trianglePhaseTwo + 1 + currentTriangleSlopeTwo))) * currentTriangleVolumeTwo);
            }
            else if (this->globalParameters->oscTypeTwo == 3)
            {
                sampleTwo = (SamplePrecision)(this->globalParameters->noiseBufferTwo->at (noisePosTwo) * currentNoiseVolumeTwo);
            }
        
            
            
            //filter two
            if (filterTwoFreqRamp != 0. || filterTwoQRamp != 0.)
            {
                filterTwo->setFreqAndQ (VoiceStatics::freqLogScale.scale (currentLPTwoFreq), 1. - currentLPTwoQ);
                currentLPTwoFreq += filterTwoFreqRamp;
                currentLPTwoQ += filterTwoQRamp;
            }
            sampleTwo = (SamplePrecision)filterTwo->process (sampleTwo);
            
            
            SamplePrecision osc = (SamplePrecision)sin (n * triangleFreq + trianglePhase);
            if (this->globalParameters->oscType == 0)
            {
                sample = (SamplePrecision)(sin (n * sinusFreq + sinusPhase) * currentSinusVolume);
            }
            else if (this->globalParameters->oscType == 1)
            {
                sample = (SamplePrecision)((::floor (osc) + 0.5) * currentSquareVolume);
            }
            else if (this->globalParameters->oscType == 2)
            {
                sample = (SamplePrecision)((osc - ::fabs(sin (n * triangleFreq + trianglePhase + 1 + currentTriangleSlope))) * currentTriangleVolume);
            }
            else if (this->globalParameters->oscType == 3)
            {
                sample = (SamplePrecision)(this->globalParameters->noiseBuffer->at (noisePos) * currentNoiseVolume);
            }

            
            //filter
            if (filterOneFreqRamp != 0. || filterOneQRamp != 0.)
            {
                filterOne->setFreqAndQ (VoiceStatics::freqLogScale.scale (currentLPOneFreq), 1. - currentLPOneQ);
                currentLPOneFreq += filterOneFreqRamp;
                currentLPOneQ += filterOneQRamp;
            }
            sample = (SamplePrecision)filterOne->process (sample);
            
            //add together two samples after two filters
            sample += sampleTwo;
            
			n++;
			
			// add noise
			//sample += (SamplePrecision)(this->globalParameters->noiseBuffer->at (noisePos) * currentNoiseVolume);

			// filter
			if (filterFreqRamp != 0. || filterQRamp != 0.)
			{
				filter->setFreqAndQ (VoiceStatics::freqLogScale.scale (currentLPFreq), 1. - currentLPQ);
				currentLPFreq += filterFreqRamp;
				currentLPQ += filterQRamp;
			}
			sample = (SamplePrecision)filter->process (sample);

			// store in output
			outputBuffers[0][i] += (SamplePrecision)(sample * currentPanningLeft * currentVolume);
			outputBuffers[1][i] += (SamplePrecision)(sample * currentPanningRight * currentVolume);

			// advance noise
			noisePos += noiseStep;
			if (noisePos > this->globalParameters->noiseBuffer->getSize () - 2)
			{
				noisePos = (int32)((float)::rand () / (float)RAND_MAX * (this->globalParameters->noiseBuffer->getSize () - 2) + 2);		
				noiseStep = -1;
			}
			else if (noisePos < 2)
			{
				noiseStep = 1;
			}
            
            // advance noise
            noisePosTwo += noiseStepTwo;
            if (noisePosTwo > this->globalParameters->noiseBufferTwo->getSize () - 2)
            {
                noisePosTwo = (int32)((float)::rand () / (float)RAND_MAX * (this->globalParameters->noiseBufferTwo->getSize () - 2) + 2);
                noiseStepTwo = -1;
            }
            else if (noisePosTwo < 2)
            {
                noiseStepTwo = 1;
            }

			// ramp parameters
			currentVolume += volumeRamp;
			currentPanningLeft += panningLeftRamp;
			currentPanningRight += panningRightRamp;
			currentNoiseVolume += noiseVolumeRamp;
            currentNoiseVolumeTwo += noiseVolumeRampTwo;
			currentSinusVolume += sinusVolumeRamp;
            currentSinusVolumeTwo += sinusVolumeRampTwo;
			currentSquareVolume += squareVolumeRamp;
            currentSquareVolumeTwo += squareVolumeRampTwo;
			currentTriangleVolume += triangleVolumeRamp;
            currentTriangleVolumeTwo += triangleVolumeRampTwo;
			currentTriangleSlope += triangleSlopeRamp;
            currentTriangleSlopeTwo += triangleSlopeRampTwo;
		}
	}
	return true;
}

    
//STARTED SECOND GENERATOR AGAIN
//-----------------------------------------------------------------------------
template<class SamplePrecision>
void Voice<SamplePrecision>::noteOn (int32 _pitch, ParamValue velocity, float tuning, int32 sampleOffset, int32 nId)
{	
	currentVolume = 0;
	this->values[kVolumeMod] = 0;
	levelFromVel = 1.f + this->globalParameters->velToLevel * (velocity - 1.);

	currentSinusVolume = this->values[kSinusVolume] = this->globalParameters->sinusVolume;
	currentTriangleVolume = this->values[kTriangleVolume] = this->globalParameters->triangleVolume;
	currentNoiseVolume = this->values[kNoiseVolume] = this->globalParameters->noiseVolume;
	currentTriangleSlope = this->values[kTriangleSlope] = this->globalParameters->triangleSlop;
	currentSquareVolume = this->values[kSquareVolume] = this->globalParameters->squareVolume;
    currentSinusVolumeTwo = this->values[kSinusVolumeTwo] = this->globalParameters->sinusVolumeTwo;
    currentTriangleVolumeTwo = this->values[kTriangleVolumeTwo] = this->globalParameters->triangleVolumeTwo;
    currentNoiseVolumeTwo = this->values[kNoiseVolumeTwo] = this->globalParameters->noiseVolumeTwo;
    currentTriangleSlopeTwo = this->values[kTriangleSlopeTwo] = this->globalParameters->triangleSlopTwo;
    currentSquareVolumeTwo = this->values[kSquareVolumeTwo] = this->globalParameters->squareVolumeTwo;
    currentSustainVolume = this->values[kSustainVolumeMod] = this->globalParameters->sustainVolume;
	
	// filter setting
	currentLPFreq = this->globalParameters->filterFreq;
	this->values[kFilterFrequencyMod] = 0;
	currentLPQ =  this->globalParameters->filterQ;
	this->values[kFilterQMod] = 0;

	filter->setType ((Filter::Type)this->globalParameters->filterType);
	filter->setFreqAndQ (VoiceStatics::freqLogScale.scale (currentLPFreq), 1. - currentLPQ);
    
    // filter One setting
    currentLPOneFreq = this->globalParameters->filterOneFreq;
    this->values[kFilterOneFrequencyMod] = 0;
    currentLPOneQ =  this->globalParameters->filterOneQ;
    this->values[kFilterOneQMod] = 0;
    
    filterOne->setType ((Filter::Type)this->globalParameters->filterOneType);
    filterOne->setFreqAndQ (VoiceStatics::freqLogScale.scale (currentLPOneFreq), 1. - currentLPOneQ);
    
    // filter Two setting
    currentLPTwoFreq = this->globalParameters->filterTwoFreq;
    this->values[kFilterTwoFrequencyMod] = 0;
    currentLPTwoQ =  this->globalParameters->filterTwoQ;
    this->values[kFilterTwoQMod] = 0;
    
    filterTwo->setType ((Filter::Type)this->globalParameters->filterTwoType);
    filterTwo->setFreqAndQ (VoiceStatics::freqLogScale.scale (currentLPTwoFreq), 1. - currentLPTwoQ);

	currentSinusDetune = 0.;
	if (this->globalParameters->sinusDetune != 0.)
	{
		currentSinusDetune = VoiceStatics::freqTab[this->pitch] * (::pow (2.0, this->globalParameters->sinusDetune * 2.0 / 12.0) - 1);
	}
	this->values[kSinusDetune] = currentSinusDetune;
	this->values[kTuningMod] = 0;
    
    
    currentSinusDetuneTwo = 0.;
    if (this->globalParameters->sinusDetuneTwo != 0.)
    {
        currentSinusDetuneTwo = VoiceStatics::freqTab[this->pitch] * (::pow (2.0, this->globalParameters->sinusDetuneTwo * 2.0 / 12.0) - 1);
    }
    this->values[kSinusDetuneTwo] = currentSinusDetuneTwo;
    this->values[kTuningMod] = 0; //DO I NEED 2???
    
    //////atttaaacckkkk---------------------------
    
    ParamValue timeFactor;
    //if (!decayStart)
    //{
        if (this->values[kAttackTimeMod] == 0)
            timeFactor = 1;
        else
            timeFactor = ::pow (100., this->values[kAttackTimeMod]);
        
        noteOnVolumeRamp = 1.0 / (timeFactor * this->sampleRate * ((this->globalParameters->attackTime * MAX_ATTACK_TIME_SEC) + 0.005));
        if (currentVolume)
            noteOnVolumeRamp *= currentVolume;
        //////////////////-----------------------------
    //}
    //else
    //{
    ParamValue timeFactorDecay;
        //////deeeecccccaaaaaayyyy---------------------------
        if (this->values[kDecayTimeMod] == 0)
            timeFactorDecay = 1;
        else
            timeFactorDecay = ::pow (100., this->values[kDecayTimeMod]);
        
        noteOnVolumeRampDecay = 1.0 / (timeFactorDecay * this->sampleRate * ((this->globalParameters->decayTime * MAX_DECAY_TIME_SEC) + 0.005));
        if (currentVolume)
            noteOnVolumeRampDecay *= currentVolume;
        //////////////////-----------------------------
    //}
    
	VoiceBase<kNumParameters, SamplePrecision, 2, GlobalParameterState>::noteOn (_pitch, velocity, tuning, sampleOffset, nId);
	this->noteOnSampleOffset++;
}

//-----------------------------------------------------------------------------
template<class SamplePrecision>
void Voice<SamplePrecision>::noteOff (ParamValue velocity, int32 sampleOffset)
{
	VoiceBase<kNumParameters, SamplePrecision, 2, GlobalParameterState>::noteOff (velocity, sampleOffset);
	this->noteOffSampleOffset++;

	ParamValue timeFactor;
	if (this->values[kReleaseTimeMod] == 0)
		timeFactor = 1;
	else
		timeFactor = ::pow (100., this->values[kReleaseTimeMod]);
	
	noteOffVolumeRamp = 1.0 / (timeFactor * this->sampleRate * ((this->globalParameters->releaseTime * MAX_RELEASE_TIME_SEC) + 0.005));
	if (currentVolume)
		noteOffVolumeRamp *= currentVolume;
}

//-----------------------------------------------------------------------------
template<class SamplePrecision>
void Voice<SamplePrecision>::reset ()
{
	noiseStep = 1;
    noiseStepTwo = 1;
	noisePos = 0;
    noisePosTwo = 0;
	n = 0;
	sinusPhase = trianglePhase = 0.;
    sinusPhaseTwo = trianglePhaseTwo = 0.;
	currentSinusF = currentTriangleF = -1.;
    currentSinusFTwo = currentTriangleFTwo = -1.;
	this->values[kVolumeMod] = 0.;
	this->values[kTuningMod] = 0.;
	this->values[kFilterFrequencyMod] = 0.;
    this->values[kFilterOneFrequencyMod] = 0.;
    this->values[kFilterTwoFrequencyMod] = 0.;
	this->values[kFilterQMod] = 0.;
    this->values[kFilterOneQMod] = 0.;
    this->values[kFilterTwoQMod] = 0.;
	this->values[kReleaseTimeMod] = 0.;
	this->values[kAttackTimeMod] = 0.;
    this->values[kSustainVolumeMod] = 0.;
    this->values[kDecayTimeMod] = 0.;
    this->values[kGenFreqOne] = 0.;
    this->values[kGenFreqTwo] = 0.5;
	currentPanningLeft = this->values[kPanningLeft] = 1.;
	currentPanningRight = this->values[kPanningRight] = 1.;
	currentNoiseVolume = this->values[kNoiseVolume] = 0.5;
    currentNoiseVolumeTwo = this->values[kNoiseVolumeTwo] = 0.5;
	currentSinusVolume = this->values[kSinusVolume] = 0.5;
    currentSinusVolumeTwo = this->values[kSinusVolumeTwo] = 0.5;
	currentSquareVolume = this->values[kSquareVolume] = 0.5;
    currentSquareVolumeTwo = this->values[kSquareVolumeTwo] = 0.5;
	currentTriangleVolume = this->values[kTriangleVolume] = 0.5;
    currentTriangleVolumeTwo = this->values[kTriangleVolumeTwo] = 0.5;
	currentLPFreq = 1.;
	currentLPQ = 0.;
    currentLPOneFreq = 1.;
    currentLPOneQ = 0.;
    currentLPTwoFreq = 1.;
    currentLPTwoQ = 0.;
	filter->reset ();
    filterOne->reset ();
    filterTwo->reset ();
	noteOffVolumeRamp = 0.005;
	
	VoiceBase<kNumParameters, SamplePrecision, 2, GlobalParameterState>::reset ();
}

//-----------------------------------------------------------------------------
template<class SamplePrecision>
void Voice<SamplePrecision>::setSampleRate (ParamValue sampleRate)
{
	filter->setSampleRate (sampleRate);
    filterOne->setSampleRate (sampleRate);
    filterTwo->setSampleRate (sampleRate);
	VoiceBase<kNumParameters, SamplePrecision, 2, GlobalParameterState>::setSampleRate (sampleRate);
}

//-----------------------------------------------------------------------------
template<class SamplePrecision>
Voice<SamplePrecision>::Voice ()
{
	filter = new Filter (Filter::kLowpass);
    filterOne = new Filter (Filter::kLowpass);
    filterTwo = new Filter (Filter::kLowpass);
}

//-----------------------------------------------------------------------------
template<class SamplePrecision>
Voice<SamplePrecision>::~Voice ()
{
	delete filter;
    delete filterOne;
    delete filterTwo;
}

}}} // namespaces
