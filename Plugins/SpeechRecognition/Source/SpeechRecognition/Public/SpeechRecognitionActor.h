
#pragma once

#include "SpeechRecognitionWorker.h"
#include "SpeechRecognition.h"
#include "Async/TaskGraphInterfaces.h"
#include "Sound/SoundWaveProcedural.h"
#include "SpeechRecognitionActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSpeechRecognitionActiveSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStartedSpeakingSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStoppedSpeakingSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUnknownPhraseSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWordsSpokenSignature, FRecognisedPhrases, Text);

UCLASS(BlueprintType, Blueprintable)
class SPEECHRECOGNITION_API ASpeechRecognitionActor : public AActor
{
	GENERATED_BODY()

private:

	int32 instanceCtr;
	
	FSpeechRecognitionWorker* listenerThread;

	void CreateVoiceProcedural();

	static void SpeechRecognitionActive_trigger(FSpeechRecognitionActiveSignature delegate_method);
	static void StartedSpeaking_trigger(FStartedSpeakingSignature delegate_method);
	static void StoppedSpeaking_trigger(FStoppedSpeakingSignature delegate_method);
	static void UnknownPhrase_trigger(FUnknownPhraseSignature delegate_method);
	static void WordsSpoken_trigger(FWordsSpokenSignature delegate_method, FRecognisedPhrases text);

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Voice")
	USoundWaveProcedural* VoiceCaptureSoundWaveProcedural;

	//Methods to switch recognition modes
	UFUNCTION(BlueprintCallable, Category = "Audio", meta = (DisplayName = "Enable Keyword Mode", Keywords = "Speech Recognition Mode"))
	bool EnableKeywordMode(TArray<FRecognitionPhrase> wordList);

	UFUNCTION(BlueprintCallable, Category = "Audio", meta = (DisplayName = "Enable Grammar Mode", Keywords = "Speech Recognition Mode"))
	bool EnableGrammarMode(FString grammarName);

	UFUNCTION(BlueprintCallable, Category = "Audio", meta = (DisplayName = "Enable Language Model Mode", Keywords = "Speech Recognition Mode"))
	bool EnableLanguageModel(FString languageModel);

	// Basic functions 
	UFUNCTION(BlueprintCallable, Category = "Audio", meta = (DisplayName = "GetCurrentVolume", Keywords = "Speech Recognition Volume"))
	int32 GetCurrentVolume();

	UFUNCTION(BlueprintCallable, Category = "Audio", meta = (DisplayName = "Init", Keywords = "Speech Recognition Init"))
	bool Init(ESpeechRecognitionLanguage language);	

	UFUNCTION(BlueprintCallable, Category = "Audio", meta = (DisplayName = "SetConfigParam", Keywords = "Speech Recognition Set Config Param"))
	bool SetConfigParam(FString param, ESpeechRecognitionParamType type, FString value);

	UFUNCTION(BlueprintCallable, Category = "Audio", meta = (DisplayName = "Shutdown", Keywords = "Speech Recognition Shutdown"))
	bool Shutdown();

	// Used for real-time procedural sound wave
	void UpdateVoiceProcedural(vector<int16> audioData);
	
	UFUNCTION(BlueprintCallable, Category = "Audio", meta = (DisplayName = "Reset Soundwave", Keywords = "Microphone recording"))
	void ResetSoundWaveProcedural();
	

	/*
	TODO: Investigate, and correct at a later date
	// Used to obtain a procedural sound-wave, between the start and end times
	UFUNCTION(BlueprintCallable, Category = "Audio", meta = (DisplayName = "Get Microphone Snippet", Keywords = "Microphone recording"))
	USoundWaveProcedural* GetSoundWave(float startTime, float endTime);
	*/

	// Callback events
	UFUNCTION()
	void SpeechRecognitionActive_method();
	
	UPROPERTY(BlueprintAssignable, Category = "Audio")
	FSpeechRecognitionActiveSignature OnSpeechRecognitionActive;

	UFUNCTION()
	void StartedSpeaking_method();

	UPROPERTY(BlueprintAssignable, Category = "Audio")
	FStartedSpeakingSignature OnStartedSpeaking;

	UFUNCTION()
	void StoppedSpeaking_method();

	UPROPERTY(BlueprintAssignable, Category = "Audio")
	FStoppedSpeakingSignature OnStoppedSpeaking;

	UFUNCTION()
	void UnknownPhrase_method();

	UPROPERTY(BlueprintAssignable, Category = "Audio")
	FUnknownPhraseSignature OnUnknownPhrase;

	UFUNCTION()
	void WordsSpoken_method(FRecognisedPhrases phrases);

	UPROPERTY(BlueprintAssignable, Category = "Audio")
	FWordsSpokenSignature OnWordsSpoken;

};
