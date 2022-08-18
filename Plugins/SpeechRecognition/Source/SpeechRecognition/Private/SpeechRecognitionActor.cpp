
#include "SpeechRecognitionActor.h"
#include "SpeechRecognition.h"

#define SPEECHRECOGNITIONPLUGIN ISpeechRecognition::Get()

bool ASpeechRecognitionActor::Init(ESpeechRecognitionLanguage language)
{
	// terminate any existing thread
	if (listenerThread != NULL)
		Shutdown();

	// start listener thread
	listenerThread = new FSpeechRecognitionWorker();
	TArray<FRecognitionPhrase> dictionaryList;
	listenerThread->SetLanguage(language);
	bool threadSuccess = listenerThread->StartThread(this);

	return threadSuccess;
}

int32 ASpeechRecognitionActor::GetCurrentVolume()
{
	if (listenerThread != NULL) {
		int16 returnVal = listenerThread->GetCurrentVolume();
		return returnVal;
	}
	return false;
}

void  ASpeechRecognitionActor::CreateVoiceProcedural()
{
	// init voice capture sound wave procedural
	if (VoiceCaptureSoundWaveProcedural != NULL) {
		VoiceCaptureSoundWaveProcedural->RemoveFromRoot();
	}
	VoiceCaptureSoundWaveProcedural = NewObject<USoundWaveProcedural>();
	//VoiceCaptureSoundWaveProcedural->SampleRate = 16000;
	VoiceCaptureSoundWaveProcedural->NumChannels = 1;
	VoiceCaptureSoundWaveProcedural->Duration = INDEFINITELY_LOOPING_DURATION;
	VoiceCaptureSoundWaveProcedural->SoundGroup = SOUNDGROUP_Voice;
	VoiceCaptureSoundWaveProcedural->bLooping = false;
	VoiceCaptureSoundWaveProcedural->bProcedural = true;
	VoiceCaptureSoundWaveProcedural->Pitch = 1.0f;
	VoiceCaptureSoundWaveProcedural->Volume = 5.f;
	VoiceCaptureSoundWaveProcedural->AddToRoot();
}


void ASpeechRecognitionActor::UpdateVoiceProcedural(vector<int16> audioData)
{
	if (VoiceCaptureSoundWaveProcedural == NULL) {
		CreateVoiceProcedural();
	}

	TArray<uint8> audioBuffer;

	for (int16 sample : audioData) {
		uint16 uintSample = (uint16)sample;
		uint8 b1 = uintSample >> 8;
		uint8 b2 = uintSample & 0xFF;
		audioBuffer.Add(b2);
		audioBuffer.Add(b1);
	}

	// write the buffer to the raw data
	VoiceCaptureSoundWaveProcedural->QueueAudio(audioBuffer.GetData(), audioBuffer.Num());
}

void ASpeechRecognitionActor::ResetSoundWaveProcedural() {
	VoiceCaptureSoundWaveProcedural->ResetAudio();
}

/* TODO: Investigate, and correct at a later date

USoundWaveProcedural* ASpeechRecognitionActor::GetSoundWave(float startTime, float endTime)
{
	USoundWaveProcedural* soundWave = NULL;
	if (listenerThread != NULL) {
		vector<int16> audioData = listenerThread->GetAudioBytes(startTime, endTime);

		soundWave = NewObject<USoundWaveProcedural>();
		soundWave->SampleRate = 16000;
		soundWave->NumChannels = 1;
		soundWave->Duration = (endTime - startTime);
		soundWave->SoundGroup = SOUNDGROUP_Voice;
		soundWave->bLooping = false;
		soundWave->bProcedural = true;
		soundWave->Pitch = 1.0f;
		soundWave->Volume = 5.f;
		//soundWave->AddToRoot();

		TArray<uint8> audioBuffer;

		for (int16 sample : audioData) {
			uint16 uintSample = (uint16)sample;
			uint8 b1 = uintSample >> 8;
			uint8 b2 = uintSample & 0xFF;
			audioBuffer.Add(b2);
			audioBuffer.Add(b1);
		}
		soundWave->QueueAudio(audioBuffer.GetData(), audioBuffer.Num());
	}
	return soundWave;
}
*/

bool ASpeechRecognitionActor::SetConfigParam(FString param, ESpeechRecognitionParamType type, FString value)
{
	if (listenerThread != NULL) {
		bool returnVal = listenerThread->SetConfigParam(param, type, value);
		return returnVal;
	}
	return false;
}

bool ASpeechRecognitionActor::Shutdown()
{
	if (listenerThread != NULL) {
		listenerThread->ShutDown();
		listenerThread = NULL;
		return true;
	}
	else{
		return false;
	}
}

/**************************
// Change recognition methods
**************************/
bool ASpeechRecognitionActor::EnableKeywordMode(TArray<FRecognitionPhrase> wordList)
{
	if (listenerThread != NULL) {
		return listenerThread->EnableKeywordMode(wordList);
	}
	return false;
}

bool ASpeechRecognitionActor::EnableGrammarMode(FString grammarName)
{
	if (listenerThread != NULL) {
		return listenerThread->EnableGrammarMode(grammarName);
	}
	return false;
}

bool ASpeechRecognitionActor::EnableLanguageModel(FString FlanguageModel)
{
	if (listenerThread != NULL) {
		return listenerThread->EnableLanguageModel(FlanguageModel);
	}
	return false;
}

/**************************
// Callback methods
**************************/

void ASpeechRecognitionActor::SpeechRecognitionActive_trigger(FSpeechRecognitionActiveSignature delegate_method)
{
	delegate_method.Broadcast();
}

void ASpeechRecognitionActor::SpeechRecognitionActive_method()
{
	FSimpleDelegateGraphTask::CreateAndDispatchWhenReady
	(
		FSimpleDelegateGraphTask::FDelegate::CreateStatic(&SpeechRecognitionActive_trigger, OnSpeechRecognitionActive)
		, TStatId()
		, nullptr
		, ENamedThreads::GameThread
	);
}

void ASpeechRecognitionActor::WordsSpoken_trigger(FWordsSpokenSignature delegate_method, FRecognisedPhrases text)
{
	delegate_method.Broadcast(text);
}

void ASpeechRecognitionActor::WordsSpoken_method(FRecognisedPhrases text)
{
	FSimpleDelegateGraphTask::CreateAndDispatchWhenReady
		(
			FSimpleDelegateGraphTask::FDelegate::CreateStatic(&WordsSpoken_trigger, OnWordsSpoken, text)
			, TStatId()
			, nullptr
			, ENamedThreads::GameThread
			);
}

void ASpeechRecognitionActor::UnknownPhrase_trigger(FUnknownPhraseSignature delegate_method)
{
	delegate_method.Broadcast();
}

void ASpeechRecognitionActor::UnknownPhrase_method()
{
	FSimpleDelegateGraphTask::CreateAndDispatchWhenReady
		(
			FSimpleDelegateGraphTask::FDelegate::CreateStatic(&UnknownPhrase_trigger, OnUnknownPhrase)
			, TStatId()
			, nullptr
			, ENamedThreads::GameThread
			);
}

void ASpeechRecognitionActor::StartedSpeaking_trigger(FStartedSpeakingSignature delegate_method)
{
	delegate_method.Broadcast();
}

void ASpeechRecognitionActor::StartedSpeaking_method()
{
	FSimpleDelegateGraphTask::CreateAndDispatchWhenReady
		(
			FSimpleDelegateGraphTask::FDelegate::CreateStatic(&StartedSpeaking_trigger, OnStartedSpeaking)
			, TStatId()
			, nullptr
			, ENamedThreads::GameThread
			);
}

void ASpeechRecognitionActor::StoppedSpeaking_trigger(FStoppedSpeakingSignature delegate_method)
{
	delegate_method.Broadcast();
}

void ASpeechRecognitionActor::StoppedSpeaking_method()
{
	FSimpleDelegateGraphTask::CreateAndDispatchWhenReady
		(
			FSimpleDelegateGraphTask::FDelegate::CreateStatic(&StoppedSpeaking_trigger, OnStoppedSpeaking)
			, TStatId()
			, nullptr
			, ENamedThreads::GameThread
			);
}
