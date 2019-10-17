//-----------------------------------------------------------------------------
// Project     : VST SDK
//
// Category    : Examples
// Filename    : public.sdk/samples/vst/note_expression_synth/source/note_expression_synth_processor.cpp
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

#include "note_expression_synth_processor.h"
#include "../../common/voiceprocessor.h"
#include "note_expression_synth_controller.h"
#include "pluginterfaces/base/ustring.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"
#include "note_expression_synth_voice.h"
#include <algorithm>
#include <iostream>
#include <fstream>

namespace Steinberg {
namespace Vst {
namespace NoteExpressionSynth {

//-----------------------------------------------------------------------------
FUID Processor::cid (0x6EE65CD1, 0xB83A4AF4, 0x80AA7929, 0xAEA6B8A0);

//-----------------------------------------------------------------------------
Processor::Processor () : voiceProcessor (nullptr)
{
	setControllerClass (Controller::cid);

	memset (&paramState, 0, sizeof (paramState));

	paramState.masterVolume = 0.8;
	paramState.masterTuning = 0.0;
	paramState.velToLevel = 1.;

	paramState.noiseVolume = 0.1;
    paramState.squareVolume = 0.1;
	paramState.sinusVolume = 0.8;
	paramState.triangleVolume= 1.;
    paramState.noiseVolumeTwo = 0.1;
    paramState.squareVolumeTwo = 0.1;
    paramState.sinusVolumeTwo = 0.8;
    paramState.triangleVolumeTwo = 1.;
	paramState.releaseTime = 0.;
	paramState.attackTime = 0.;
    paramState.sustainVolume = 0.8;
    paramState.decayTime = 0.;
	paramState.sinusDetune = 0.0;
	paramState.triangleSlop = 0.5;
    paramState.sinusDetuneTwo = 0.0;
    paramState.triangleSlopTwo = 0.5;
	paramState.filterFreq = 1.0;
	paramState.filterQ = 0.0;
	paramState.filterType = 0;
	paramState.freqModDepth = 1.;
    paramState.filterOneFreq = 1.0;
    paramState.filterOneQ = 0.0;
    paramState.filterOneType = 0;
    paramState.freqOneModDepth = 1.;
    paramState.filterTwoFreq = 1.0;
    paramState.filterTwoQ = 0.0;
    paramState.filterTwoType = 0;
    paramState.freqTwoModDepth = 1.;
	paramState.bypassSNA = 0;
    paramState.oscType = 0;
    paramState.oscTypeTwo = 0;
    paramState.genFreqOne = 0.0;
    paramState.genFreqTwo = 0.0;
    paramState.stereoMs = 0.0;
	paramState.freqModOn = 0.0;
    paramState.saveState = 0.0;
    paramState.loadState = 0.0;
    
    
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API Processor::initialize (FUnknown* context)
{
	tresult result = AudioEffect::initialize (context);
	if (result == kResultTrue)
	{
		addAudioOutput (STR16 ("Audio Output"), SpeakerArr::kStereo);
		addEventInput (STR16 ("Event Input"), 1);
	}
	return result;
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API Processor::setState (IBStream* state)
{
	return paramState.setState (state);
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API Processor::getState (IBStream* state)
{
	return paramState.getState (state);
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API Processor::setBusArrangements (SpeakerArrangement* inputs, int32 numIns,
                                                  SpeakerArrangement* outputs, int32 numOuts)
{
	// we only support one stereo output bus
	if (numIns == 0 && numOuts == 1 && outputs[0] == SpeakerArr::kStereo)
	{
		return AudioEffect::setBusArrangements (inputs, numIns, outputs, numOuts);
	}
	return kResultFalse;
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API Processor::canProcessSampleSize (int32 symbolicSampleSize)
{
	if (symbolicSampleSize == kSample32 || symbolicSampleSize == kSample64)
	{
		return kResultTrue;
	}
	return kResultFalse;
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API Processor::setActive (TBool state)
{
	if (state)
	{
		if (paramState.noiseBuffer == nullptr)
			paramState.noiseBuffer = new BrownNoise<float> ((int32)processSetup.sampleRate,
			                                                (float)processSetup.sampleRate);
        if (paramState.noiseBufferTwo == nullptr)
            paramState.noiseBufferTwo = new BrownNoise<float> ((int32)processSetup.sampleRate,
                                                            (float)processSetup.sampleRate);
		if (voiceProcessor == nullptr)
		{
			if (processSetup.symbolicSampleSize == kSample32)
			{
				voiceProcessor =
				    new VoiceProcessorImplementation<float, Voice<float>, 2, MAX_VOICES,
				                                     GlobalParameterState> (
				        (float)processSetup.sampleRate, &paramState);
			}
			else if (processSetup.symbolicSampleSize == kSample64)
			{
				voiceProcessor =
				    new VoiceProcessorImplementation<double, Voice<double>, 2, MAX_VOICES,
				                                     GlobalParameterState> (
				        (float)processSetup.sampleRate, &paramState);
			}
			else
			{
				return kInvalidArgument;
			}
		}
	}
	else
	{
		if (voiceProcessor)
		{
			delete voiceProcessor;
		}
		voiceProcessor = nullptr;
		if (paramState.noiseBuffer)
		{
			delete paramState.noiseBuffer;
		}
		paramState.noiseBuffer = nullptr;
        if (paramState.noiseBufferTwo)
        {
            delete paramState.noiseBufferTwo;
        }
        paramState.noiseBufferTwo = nullptr;
	}
	return AudioEffect::setActive (state);
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API Processor::process (ProcessData& data)
{
	// TODO: maybe try to make this nearly sample accurate
	if (data.inputParameterChanges)
	{
		int32 count = data.inputParameterChanges->getParameterCount ();
		for (int32 i = 0; i < count; i++)
		{
			IParamValueQueue* queue = data.inputParameterChanges->getParameterData (i);
			if (queue)
			{
				int32 sampleOffset;
				ParamValue value;
				ParamID pid = queue->getParameterId ();
				if (queue->getPoint (queue->getPointCount () - 1, sampleOffset, value) ==
				    kResultTrue)
				{
					switch (pid)
					{
						case kParamMasterVolume:
						{
							paramState.masterVolume = value;
							break;
						}
						case kParamMasterTuning:
						{
							paramState.masterTuning = 2 * (value - 0.5);
							break;
						}
						case kParamVelToLevel:
						{
							paramState.velToLevel = value;
							break;
						}
						case kParamFilterFreqModDepth:
						{
							paramState.freqModDepth = 2 * (value - 0.5);
							break;
						}
                        case kParamFilterOneFreqModDepth:
                        {
                            paramState.freqOneModDepth = 2 * (value - 0.5);
                            break;
                        }
                        case kParamFilterTwoFreqModDepth:
                        {
                            paramState.freqTwoModDepth = 2 * (value - 0.5);
                            break;
                        }

						case kParamNoiseVolume:
						{
							paramState.noiseVolume = value;
							break;
						}
                        case kParamNoiseVolumeTwo:
                        {
                            paramState.noiseVolumeTwo = value;
                            break;
                        }
						case kParamSinusVolume:
						{
							paramState.sinusVolume = value;
							break;
						}
                        case kParamSinusVolumeTwo:
                        {
                            paramState.sinusVolumeTwo = value;
                            break;
                        }
						case kParamTriangleVolume:
						{
							paramState.triangleVolume = value;
							break;
						}
                        case kParamTriangleVolumeTwo:
                        {
                            paramState.triangleVolumeTwo = value;
                            break;
                        }
						case kParamSquareVolume:
						{
							paramState.squareVolume = value;
							break;
						}
                        case kParamSquareVolumeTwo:
                        {
                            paramState.squareVolumeTwo = value;
                            break;
                        }
						case kParamReleaseTime:
						{
							paramState.releaseTime = value;
							break;
						}
						case kParamAttackTime:
						{
							paramState.attackTime = value;
							break;
						}
                        case kParamSustainVolume:
                        {
                            paramState.sustainVolume = value;
                            break;
                        }
                        case kParamDecayTime:
                        {
                            paramState.decayTime = value;
                            break;
                        }
						case kParamSinusDetune:
						{
							paramState.sinusDetune = 2 * (value - 0.5);
							break;
						}

						case kParamTriangleSlop:
						{
							paramState.triangleSlop = value;
							break;
						}
                        case kParamTriangleSlopTwo:
                        {
                            paramState.triangleSlopTwo = value;
                            break;
                        }
                        case kParamGenFreqOne:
                        {
                            paramState.genFreqOne = value;
                            break;
                        }
                        case kParamGenFreqTwo:
                        {
                            paramState.genFreqTwo = value;
                            break;
                        }
                        case kParamStereoMs:
                        {
                            paramState.stereoMs = value;
                            break;
                        }
						case kParamFreqModOn:
						{
							paramState.freqModOn = value;
							break;
						}
                        case kParamSaveState:
                        {
							//voice.save();
							std::ofstream myfile("D:/Documents/values.txt");
							std::string data(std::to_string(paramState.masterVolume));
							myfile << data;
							myfile.flush();
							myfile.close();
                            break;
                        }
                        case kParamLoadState:
                        {
							std::string line;
							std::ifstream myfile("D:/Documents/values.txt");
							while (std::getline(myfile, line))
							{
								paramState.masterVolume = std::stof(line);
								if (data.outputParameterChanges)
								{
									int32 index;
									IParamValueQueue* queue =
										data.outputParameterChanges->addParameterData(kParamMasterVolume, index);
									if (queue)
									{
										queue->addPoint(
											0, paramState.masterVolume, index);
									}
								}
							}
							myfile.close();
                            break;
                        }
						case kParamFilterType:
						{
							paramState.filterType = std::min<int8> (
							    (int8) (NUM_FILTER_TYPE * value), NUM_FILTER_TYPE - 1);
							break;
						}
                        case kParamOscType:
                        {
                            paramState.oscType = std::min<int8> (
                                                                 (int8) (NUM_OSC_TYPE * value), NUM_OSC_TYPE - 1);
                            break;
                        }
                        case kParamOscTypeTwo:
                        {
                            paramState.oscTypeTwo = std::min<int8> (
                                                                    (int8) (NUM_OSC_TYPE_TWO * value), NUM_OSC_TYPE_TWO - 1);
                            break;
                        }
						case kParamFilterFreq:
						{
							paramState.filterFreq = value;
							break;
						}
						case kParamFilterQ:
						{
							paramState.filterQ = value;
							break;
						}
                        case kParamFilterOneType:
                        {
                            paramState.filterOneType = std::min<int8> (
                                                                    (int8) (NUM_FILTER_TYPE * value), NUM_FILTER_TYPE - 1);
                            break;
                        }
                        case kParamFilterOneFreq:
                        {
                            paramState.filterOneFreq = value;
                            break;
                        }
                        case kParamFilterOneQ:
                        {
                            paramState.filterOneQ = value;
                            break;
                        }
                        case kParamFilterTwoType:
                        {
                            paramState.filterTwoType = std::min<int8> (
                                                                    (int8) (NUM_FILTER_TYPE * value), NUM_FILTER_TYPE - 1);
                            break;
                        }
                        case kParamFilterTwoFreq:
                        {
                            paramState.filterTwoFreq = value;
                            break;
                        }
                        case kParamFilterTwoQ:
                        {
                            paramState.filterTwoQ = value;
                            break;
                        }
						case kParamBypassSNA:
						{
							paramState.bypassSNA = (value >= 0.5) ? 1 : 0;
							break;
						}
						case kParamTuningRange:
						{
							paramState.tuningRange = std::min<int8> (
							    (int8) (NUM_TUNING_RANGE * value), NUM_TUNING_RANGE - 1);
							break;
						}
					}
				}
			}
		}
	}
	tresult result;

	// flush mode
	if (data.numOutputs < 1)
		result = kResultTrue;
	else
		result = voiceProcessor->process (data);
	if (result == kResultTrue)
	{
		if (data.outputParameterChanges)
		{
			int32 index;
			IParamValueQueue* queue =
			    data.outputParameterChanges->addParameterData (kParamActiveVoices, index);
			if (queue)
			{
				queue->addPoint (
				    0, (ParamValue)voiceProcessor->getActiveVoices () / (ParamValue)MAX_VOICES,
				    index);
			}
		}
		if (voiceProcessor->getActiveVoices () == 0 && data.numOutputs > 0)
		{
			data.outputs[0].silenceFlags = 0x11; // left and right channel are silent
		}
	}
	return result;
}
} // NoteExpressionSynth
} // Vst
} // Steinberg
