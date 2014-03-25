#ifndef SOUNDMANAGER3D_H_
#define SOUNDMANAGER3D_H_


#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib" )

#include "../Camera/CameraManager.h"


#include <Windows.h>
#include <mmsystem.h>
#include <dsound.h>
#include <stdio.h>

#include <iostream>
#include <fstream>


class SoundManager3D
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

public:
	SoundManager3D();
	SoundManager3D(const SoundManager3D&);
	~SoundManager3D();

	bool Initialize(HWND _hwnd);
	void Shutdown();

	bool PlayWaveFile();

	float ChangeVolume(float _volume);
	bool LoadVolume(std::string _fileName);

	float SetSoundPosition(float _x, float _y, float _z);
	void UpdateListener();
	float m_fPositionX, m_fPositionY, m_fPositionZ;

private:
	bool InitializeDirectSound(HWND _hwnd);
	void ShutdownDirectSound();

	bool LoadWaveFile(char* _fileName, IDirectSoundBuffer8** _buffer, IDirectSound3DBuffer8**  _buffer3D );
	void ShutdownWaveFile(IDirectSoundBuffer8** _buffer, IDirectSound3DBuffer8** _buffer3D);

	
	
	float m_fVolume;

private:
	IDirectSound8* m_DirectSound;
	IDirectSoundBuffer* m_primaryBuffer;


	IDirectSound3DListener8* m_listener;
	IDirectSoundBuffer8* m_secondaryBuffer1;


	IDirectSound3DBuffer8* m_secondary3DBuffer1;


	
};
#endif