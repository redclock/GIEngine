#pragma once
#include <fmod.hpp>
#include <fmod_errors.h>

typedef FMOD::Sound CGameSound;

class CSoundPlayer
{

public:

    //filename播放的声音文件的路径,返回该声音对象
    //如果载入不成功，返回NULL
    CGameSound * LoadSound(const char * filename, bool loop, bool is3D = false); 

    //播放id为n的声音,返回id，若不成功，返回－1
    int Play(CGameSound * sound);       

	//强行停止id为n的通道
    void StopSound(int n);

    //更新声音播放状态,检查是否有声音已经播完,如果有,就将它停止,
    //每一帧都会调用这个函数
    void SoundLoop();

    //返回id为n的声音是否正在播放,如果没有装载id=n的声音,就返回false
    bool IsPlaying(int n);
	//初始化
	void InitSystem();
    //释放所有声音资源
    void CloseSystem();


	//构造函数  
	CSoundPlayer(int max);   
	~CSoundPlayer();
protected:

    FMOD::Sound      **sound;
    FMOD::Channel    **channel;
    FMOD::System     *system;
    unsigned int      version;
	int               maxChannel;
    void ERRCHECK(FMOD_RESULT result);
    void channelRelease(int n);
};



