#include <stdio.h>

#include "FMODSoundPlay.h"


CSoundPlayer::CSoundPlayer(int max)
{
	maxChannel = max;
	channel = new FMOD::Channel *[maxChannel];
	sound   = new FMOD::Sound *  [maxChannel];
	for (int i = 0; i < maxChannel; i++)
	{
		channel[i] = NULL;
		sound[i] = NULL;
	}
	system = NULL;
}

CSoundPlayer::~CSoundPlayer()
{
	if (system) CloseSystem();
	delete[] channel;
	delete[] sound;
}

void CSoundPlayer::InitSystem()
{
	if (system) return;
	FMOD_RESULT result = FMOD::System_Create(&system);
	ERRCHECK(result);

	result = system->getVersion(&version);
	ERRCHECK(result);

	if (version < FMOD_VERSION)
	{
		printf("Error!  You are using an old version of FMOD %08x.  This program requires %08x\n", version, FMOD_VERSION);

	}

	result = system->init(32, FMOD_INIT_NORMAL, 0);
	ERRCHECK(result);
}

void CSoundPlayer::CloseSystem()
{
	for (int i = 0;i < maxChannel; i++)
	{
		if (channel[i])	
		{							  
			channelRelease(i);
		}
	}
	FMOD_RESULT result = system->close();
	ERRCHECK(result);
	result = system->release();
	ERRCHECK(result);
	system = NULL;
}

void CSoundPlayer::ERRCHECK(FMOD_RESULT result)
{
    if (result != FMOD_OK)
    {
        printf("FMOD error! (%d) %S\n", result, FMOD_ErrorString(result));
    }
}


int CSoundPlayer::Play(CGameSound * sound)
{
	if (sound)
	{
		int i;
		for (i = 0; i < maxChannel; i++)
		{
			if (channel[i] == NULL) break;
		}
		if (i == maxChannel) return -1;
        FMOD_RESULT result = system->playSound(FMOD_CHANNEL_FREE, sound, false, &channel[i]);		
        ERRCHECK(result);
		this->sound[i] = sound;
        system->update();
		return i;
    }else
	{
		return -1;
	}

}

void CSoundPlayer::StopSound(int n)
{
	channelRelease(n);
}


CGameSound *  CSoundPlayer::LoadSound(const char * filename, bool loop, bool is3D)
{
	FMOD_MODE flag = FMOD_HARDWARE;
	if (is3D) flag |= FMOD_3D;
	CGameSound * sound = NULL;
    FMOD_RESULT result = system->createSound(filename, flag, 0, &sound);
    ERRCHECK(result);
	if (sound == NULL) 
		return NULL;
	if(loop)
	{
		result = sound->setMode(FMOD_LOOP_NORMAL);

	}
	else 
	{
		result = sound->setMode(FMOD_LOOP_OFF);
	}	
	ERRCHECK(result);
	return sound;
}

void CSoundPlayer::SoundLoop()
{
	for (int i = 0; i < maxChannel; i++)
	{	
		if (channel[i])
		{
			bool playing = false;
			channel[i]->isPlaying(&playing);
			//printf(" %d check %d\n",i,playing);
			if (playing == false)
			{
				channelRelease(i);
			}
		}
	}     
}

bool CSoundPlayer::IsPlaying(int n)
{
	if (n < 0 || n >= maxChannel) return false;
	if (channel[n]!=NULL)
	{
		bool playing;
		channel[n]->isPlaying(&playing)	;
		return(playing);
	}
	else 
		return false;
}

void CSoundPlayer::channelRelease(int n)
{
	if (n < 0 || n >= maxChannel) return;
    if (channel[n]!=NULL)
    {
		channel[n]->stop();
		//if (sound[n]) sound[n]->release();
		//printf("Release channel: %d\n", n);
		channel[n]=NULL;
		sound[n] = NULL;
    }
}

