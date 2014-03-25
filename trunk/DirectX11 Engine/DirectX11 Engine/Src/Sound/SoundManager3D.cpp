#include "SoundManager3D.h"

SoundManager3D::SoundManager3D()
{
	m_DirectSound = 0;
	m_primaryBuffer = 0;


	m_listener = 0;
	m_secondaryBuffer1 = 0;

	// Sound Position
	 m_fPositionX = CameraManager::GetCurrentCamera()->GetInversePosition().x;
	 m_fPositionY = CameraManager::GetCurrentCamera()->GetInversePosition().y;
	 m_fPositionZ = CameraManager::GetCurrentCamera()->GetInversePosition().z;

	 
	
	m_secondary3DBuffer1 = 0;

	m_fVolume = DSBVOLUME_MAX;
}
SoundManager3D::SoundManager3D(const SoundManager3D& other)
{

}
SoundManager3D::~SoundManager3D()
{

}


float SoundManager3D::ChangeVolume(float _volume)
{
	m_fVolume += _volume;

	return m_fVolume;
}
bool SoundManager3D::LoadVolume(std::string _fileName)
{
	std::fstream fin;
	
	fin.open(_fileName, std::ios_base::binary | std::ios_base::binary);

	if(fin.is_open())
	{
		fin.read((char*)&m_fVolume, sizeof(float));

	}

	return true;
}

float SoundManager3D::SetSoundPosition(float _x, float _y, float _z)
{

	m_fPositionX += _x;
	m_fPositionY += _y;
	m_fPositionZ += _z;

	return true;
}


bool SoundManager3D::Initialize(HWND _hwnd)
{

	bool result;


	result = InitializeDirectSound(_hwnd);
	if(result == false)
	{
		return false;
	}

	result = LoadWaveFile("sound02.wav", &m_secondaryBuffer1, &m_secondary3DBuffer1);
	if(result == false)
	{
		return false;
	}

	result = PlayWaveFile();
	if(result == false)
	{
		return false;
	}

	return true;
}

void SoundManager3D::Shutdown()
{
	ShutdownWaveFile(&m_secondaryBuffer1, &m_secondary3DBuffer1);

	ShutdownDirectSound();

	return;
}

bool SoundManager3D::InitializeDirectSound(HWND _hwnd)
{
	HRESULT result;
	DSBUFFERDESC bufferDesc;
	WAVEFORMATEX waveFormat;

		// Initialize the direct sound interface pointer for the default sound device.
	result = DirectSoundCreate8(NULL, &m_DirectSound, NULL);
	if(FAILED(result))
	{
		return false;
	}

	// Set the cooperative level to priority so the format of the primary sound buffer can be modified.
	result = m_DirectSound->SetCooperativeLevel(_hwnd, DSSCL_PRIORITY);
	if(FAILED(result))
	{
		return false;
	}

	// Setup the primary buffer description.
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRL3D;
	bufferDesc.dwBufferBytes = 0;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = NULL;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	// Get control of the primary sound buffer on the default sound device.
	result = m_DirectSound->CreateSoundBuffer(&bufferDesc, &m_primaryBuffer, NULL);
	if(FAILED(result))
	{
		return false;
	}

	// Setup the format of the primary sound bufffer.
	// In this case it is a .WAV file recorded at 44,100 samples per second in 16-bit stereo (cd audio format).
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 2;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	// Set the primary buffer to be the wave format specified.
	result = m_primaryBuffer->SetFormat(&waveFormat);
	if(FAILED(result))
	{
		return false;
	}

	// Obtain a listener interface.
	result = m_primaryBuffer->QueryInterface(IID_IDirectSound3DListener8, (LPVOID*)&m_listener);
	if(FAILED(result))
	{
		return false;
	}
	
	// Set the initial position of the listener to be in the middle of the scene.
	//m_listener->SetPosition(cam->GetCurrentCamera()->GetPosition().x,cam->GetCurrentCamera()->GetPosition().y,cam->GetCurrentCamera()->GetPosition().z, DS3D_IMMEDIATE);
	m_listener->SetPosition( CameraManager::GetCurrentCamera()->GetInversePosition().x,
							 CameraManager::GetCurrentCamera()->GetInversePosition().y,  
							 CameraManager::GetCurrentCamera()->GetInversePosition().z, 
							 DS3D_IMMEDIATE);

	//m_listener->GetPosition();
	return true;
}

void SoundManager3D::UpdateListener()
{
	m_listener->SetPosition( CameraManager::GetCurrentCamera()->GetInversePosition().x,
							 CameraManager::GetCurrentCamera()->GetInversePosition().y,  
							 CameraManager::GetCurrentCamera()->GetInversePosition().z, 
							 DS3D_IMMEDIATE);

}

void SoundManager3D::ShutdownDirectSound()
{

	// Release the listener interface.
	if(m_listener)
	{
		m_listener->Release();
		m_listener = 0;
	}

	// Release the primary sound buffer pointer.
	if(m_primaryBuffer)
	{
		m_primaryBuffer->Release();
		m_primaryBuffer = 0;
	}

	// Release the direct sound interface pointer.
	if(m_DirectSound)
	{
		m_DirectSound->Release();
		m_DirectSound = 0;
	}

	return;
}


bool SoundManager3D::LoadWaveFile(char* _fileName, IDirectSoundBuffer8** _buffer, IDirectSound3DBuffer8** _buffer3D)
{
	int error;
	FILE* filePtr;
	unsigned int count;
	WaveHeaderType waveFileHeader;
	WAVEFORMATEX waveFormat;
	DSBUFFERDESC bufferDesc;
	HRESULT result;
	IDirectSoundBuffer* tempBuffer;
	unsigned char* waveData;
	unsigned char* bufferPtr;
	unsigned long bufferSize;

	error = fopen_s(&filePtr, _fileName, "rb");
	if(error != 0)
	{ 
		return false;
	}

	count = fread(&waveFileHeader, sizeof(waveFileHeader), 1, filePtr);
	if(count != 1)
	{
		return false;
	}

		// Check that the chunk ID is the RIFF format.
	if((waveFileHeader.chunkId[0] != 'R') || (waveFileHeader.chunkId[1] != 'I') || 
	   (waveFileHeader.chunkId[2] != 'F') || (waveFileHeader.chunkId[3] != 'F'))
	{
		return false;
	}

	// Check that the file format is the WAVE format.
	if((waveFileHeader.format[0] != 'W') || (waveFileHeader.format[1] != 'A') ||
	   (waveFileHeader.format[2] != 'V') || (waveFileHeader.format[3] != 'E'))
	{
		return false;
	}

	// Check that the sub chunk ID is the fmt format.
	if((waveFileHeader.subChunkId[0] != 'f') || (waveFileHeader.subChunkId[1] != 'm') ||
	   (waveFileHeader.subChunkId[2] != 't') || (waveFileHeader.subChunkId[3] != ' '))
	{
		return false;
	}

	// Check that the audio format is WAVE_FORMAT_PCM.
	if(waveFileHeader.audioFormat != WAVE_FORMAT_PCM)
	{
		return false;
	}

	// Check that the wave file was recorded in mono format.
	if(waveFileHeader.numChannels != 1)
	{
		return false;
	}

	// Check that the wave file was recorded at a sample rate of 44.1 KHz.
	if(waveFileHeader.sampleRate != 44100)
	{
		return false;
	}

	// Ensure that the wave file was recorded in 16 bit format.
	if(waveFileHeader.bitsPerSample != 16)
	{
		return false;
	}

	// Check for the data chunk header.
	if((waveFileHeader.dataChunkId[0] != 'd') || (waveFileHeader.dataChunkId[1] != 'a') ||
	   (waveFileHeader.dataChunkId[2] != 't') || (waveFileHeader.dataChunkId[3] != 'a'))
	{
		return false;
	}

	// Set the wave format of secondary buffer that this wave file will be loaded onto.
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 1;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	// Set the buffer description of the secondary sound buffer that the wave file will be loaded onto.
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRL3D;
	bufferDesc.dwBufferBytes = waveFileHeader.dataSize;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = &waveFormat;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	// Create a temporary sound buffer with the specific buffer settings.
	result = m_DirectSound->CreateSoundBuffer(&bufferDesc, &tempBuffer, NULL);
	if(FAILED(result))
	{
		return false;
	}

	// Test the buffer format against the direct sound 8 interface and create the secondary buffer.
	result = tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&*_buffer);
	if(FAILED(result))
	{
		return false;
	}

	// Release the temporary buffer.
	tempBuffer->Release();
	tempBuffer = 0;

	// Move to the beginning of the wave data which starts at the end of the data chunk header.
	fseek(filePtr, sizeof(WaveHeaderType), SEEK_SET);

	// Create a temporary buffer to hold the wave file data.
	waveData = new unsigned char[waveFileHeader.dataSize];
	if(!waveData)
	{
		return false;
	}

	// Read in the wave file data into the newly created buffer.
	count = fread(waveData, 1, waveFileHeader.dataSize, filePtr);
	if(count != waveFileHeader.dataSize)
	{
		return false;
	}

	// Close the file once done reading.
	error = fclose(filePtr);
	if(error != 0)
	{
		return false;
	}

	// Lock the secondary buffer to write wave data into it.
	result = (*_buffer)->Lock(0, waveFileHeader.dataSize, (void**)&bufferPtr, (DWORD*)&bufferSize, NULL, 0, 0);
	if(FAILED(result))
	{
		return false;
	}

	// Copy the wave data into the buffer.
	memcpy(bufferPtr, waveData, waveFileHeader.dataSize);

	// Unlock the secondary buffer after the data has been written to it.
	result = (*_buffer)->Unlock((void*)bufferPtr, bufferSize, NULL, 0);
	if(FAILED(result))
	{
		return false;
	}
	
	// Release the wave data since it was copied into the secondary buffer.
	delete [] waveData;
	waveData = 0;

	// Get the 3D interface to the secondary sound buffer.
	result = (*_buffer)->QueryInterface(IID_IDirectSound3DBuffer8, (void**)&*_buffer3D);
	if(FAILED(result))
	{
		return false;
	}

	return true;

}

void SoundManager3D::ShutdownWaveFile(IDirectSoundBuffer8** _buffer, IDirectSound3DBuffer8** _buffer3D)
{
	if(*_buffer3D)
	{
		(*_buffer3D)->Release();
		*_buffer3D = 0;
	}
	if(*_buffer)
	{
		(*_buffer)->Release();
		*_buffer = 0;
	}

	return;
}

bool SoundManager3D::PlayWaveFile()
{
	HRESULT result;
	float xPos, yPos, zPos;

	//xPos = -2.0f;
	//yPos = 0.0f;
	//zPos = 0.0f;


	result = m_secondaryBuffer1->SetCurrentPosition(0);
	if(FAILED(result))
	{
		return false;
	}

	result = m_secondaryBuffer1->SetVolume(m_fVolume);
	if(FAILED(result))
	{
		return false;
	}

	m_secondary3DBuffer1->SetPosition(m_fPositionX,m_fPositionY,m_fPositionZ, DS3D_IMMEDIATE);

	result = m_secondaryBuffer1->Play(0,0,0);
	if(FAILED(result))
	{
		return false;
	}

	return true;
}