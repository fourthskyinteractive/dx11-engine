#ifndef SOUNDMANAGER_H_
#define SOUNDMANAGER_H_


#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib" )


#include <Windows.h>
#include <mmsystem.h>
#include <dsound.h>
#include <stdio.h>

#include <iostream>
#include <fstream>






class SoundManager
{
private:

	struct WaveHeaderType
	{
		char chunkId[4];
		unsigned long chunkSize;
		char format[4];
		char subChunkId[4];
		unsigned long subChunkSize;
		unsigned short audioFormat;
		unsigned short numChannels;
		unsigned long sampleRate;
		unsigned long bytesPerSecond;
		unsigned short blockAlign;
		unsigned short bitsPerSample;
		char dataChunkId[4];
		unsigned long dataSize;
	};




// Functions

	bool LoadWaveFile(char* _fileName, IDirectSoundBuffer8** _directBuffer);
	void ShutdownWaveFile(IDirectSoundBuffer8** _directBuffer);
	
	bool InitializeDirectSound(HWND _hwnd);
	void ShutdownDirectSound();
// Data members
	IDirectSound8*		   m_DirectSound;
	IDirectSoundBuffer * m_primaryBuffer;
	IDirectSoundBuffer8* m_secondaryBuffer;

	

public:

	SoundManager();
	~SoundManager();
	SoundManager(const SoundManager&);
	bool Initialize(HWND _hwnd);
	void ShutDown();

	float ChangeVolume(float _volume);
	bool LoadVolume(std::string _fileName);

	float	m_fVolume;

	bool PlayWaveFile();

};

#endif // !SOUNDMANAGER_H_
