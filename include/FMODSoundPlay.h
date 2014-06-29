#pragma once
#include <fmod.hpp>
#include <fmod_errors.h>

typedef FMOD::Sound CGameSound;

class CSoundPlayer
{

public:

    //filename���ŵ������ļ���·��,���ظ���������
    //������벻�ɹ�������NULL
    CGameSound * LoadSound(const char * filename, bool loop, bool is3D = false); 

    //����idΪn������,����id�������ɹ������أ�1
    int Play(CGameSound * sound);       

	//ǿ��ֹͣidΪn��ͨ��
    void StopSound(int n);

    //������������״̬,����Ƿ��������Ѿ�����,�����,�ͽ���ֹͣ,
    //ÿһ֡��������������
    void SoundLoop();

    //����idΪn�������Ƿ����ڲ���,���û��װ��id=n������,�ͷ���false
    bool IsPlaying(int n);
	//��ʼ��
	void InitSystem();
    //�ͷ�����������Դ
    void CloseSystem();


	//���캯��  
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



