#pragma once

#include <sphinxbase/err.h>
#include <sphinxbase/ad.h>
#include <pocketsphinx.h>
#include <stdio.h>
#include <chrono>
#include <time.h>
#include <regex>
#include <string>
#include <set>
#include <map>
#include <fstream>
#include <iostream>
#include <sstream>
#include <locale>
#include <cstdio>
#include <vector>
#include <utility>
#if PLATFORM_ANDROID
#include "Android/AndroidJNI.h"
#include "Android/AndroidApplication.h"
#endif
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#if PLATFORM_WINDOWS
#include <soundfile.h>
#endif


//General Log
DECLARE_LOG_CATEGORY_EXTERN(SpeechRecognitionPlugin, Log, All);

#define SENSCR_SHIFT 10

class ASpeechRecognitionActor;

using namespace std;


//Common structures and enumerations

// Used for storing a speech recognition param, used to initialize pocketsphinx
struct FSpeechRecognitionParam
{
	char* name;
	ESpeechRecognitionParamType type;
	char* value;

	FSpeechRecognitionParam(char* name, ESpeechRecognitionParamType type, char* value) {
		this->name = new char[strlen(name) + 1];
		strcpy(this->name, name);
		this->type = type;
		this->value = new char[strlen(value) + 1];
		strcpy(this->value, value);
	}
};

class FSpeechRecognitionWorker :public FRunnable
{

private:
	// Sphinx
	ps_decoder_t *ps = NULL;
	cmd_ln_t *config = NULL;
	ad_rec_t *ad;

	int16 adbuf[2048];

	uint8 utt_started, in_speech;
	int32 k;
	bool initRequired = false;
	bool wordsAdded = false;

	std::chrono::time_point<std::chrono::system_clock> initTimeChrono;
	std::chrono::time_point<std::chrono::system_clock> startTimeChrono;
	
	int32 fileCtr = 1;
	//Stores the current, peak volume
	int16 currentVolume = 0;

	//A set of params to apply to Sphinx initialisation
	TArray<FSpeechRecognitionParam> sphinxParams;

	//Speech detection mode
	ESpeechRecognitionMode detectionMode;
	
	//Language
	ESpeechRecognitionLanguage language;

	//Thread
	FRunnableThread* Thread;

	//Thread safe counter 
	FThreadSafeCounter StopTaskCounter;

	//Language
	const char* langStr = NULL;

	//Paths
	std::string argFilePath;
	std::string contentPath_str;
	std::string xmlPath_str;	
	std::string logPath;
	std::string modelPath;
	std::string languageModel;
	std::string dictionaryPath;

	std::map <int32, vector<int16>> rawAudio;
	//Stores the recognition keywords, along with their tolerances
	std::map <string , string> keywords;

	//Dictionary
	std::map <string, set<string>> dictionary;

	//Splits a string by whitespace
	vector<string> Split(string s);
	//Removes brackets, and 1-9 characters, from a string
	string GetOriginalString(string s);

public:

	// TODO: Change from static, to discrete.
	// This is a temporary measure during Android port.
	//Pointer to our manager
	//static ASpeechRecognitionActor* StaticManager;

#if PLATFORM_ANDROID
	static ASpeechRecognitionActor* Manager;
#endif 

#if PLATFORM_WINDOWS
	ASpeechRecognitionActor* Manager;
#endif 

	FSpeechRecognitionWorker();
	virtual ~FSpeechRecognitionWorker();

	//FRunnable interface
	virtual void Stop();
	virtual uint32 Run();

	//Methods to switch recognition modes
	bool EnableKeywordMode(TArray<FRecognitionPhrase> wordList);
	bool EnableGrammarMode(FString grammarName);
	bool EnableLanguageModel(FString languageModel);

	//Action methods
	void AddWords(TArray<FRecognitionPhrase> dictionaryList);
	int16 GetCurrentVolume();
	int16 GetAverageVolume(float startTime, float endTime);
	int16 GetAveragePitch(float startTime, float endTime);
	void InitConfig();
	bool SetConfigParam(FString param, ESpeechRecognitionParamType type, FString value);
	void SetLanguage(ESpeechRecognitionLanguage language);
	bool StartThread(ASpeechRecognitionActor* manager);
	void ShutDown();

	// Print Debug Text
	static void ClientMessage(FString txt);

	// Gets the audio bytes, between start and end time
	vector<int16> GetAudioBytes(float startTime, float endTime);

	/*
	// Saves WAV from audio data
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Save WAV"), Category = "Voice")
	void saveWAV(FString wav_file, int startTime, int endTime);
	*/
};

