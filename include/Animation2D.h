/************************************************************************/
/* Direct3D GameEngine                                                  */
/* �з�ͼƬ��2D����       ��д�� Ҧ����                                 */
/************************************************************************/

#pragma once

#include <vector>
#include <Windows.h>
#include <d3dx9.h>
#include "Image2D.h"

class CGame2D;

class CAnimation2D 
{
private:
	//ÿһ֡��λ��
	vector<RECT> m_rects;
	//��ʱ��
	float m_counter;
public:
	//ͼ��
	CImage2D * Image;
	//ÿ֡�Ĵ�С
	int	  ClipWidth; 
	int   ClipHeight;
	//�Ƿ�ֹͣ����
	bool  Stopped;
	//�Ƿ�ѭ��
	bool  Looped;
	//ÿ֡����ʱ
	float DelayPerFrame;
	//��ǰ��֡��
	int   CurIndex;
	//������ʼ֡��
	int   BeginFrame;
	//��������֡��
	int   EndFrame;

	void  Draw(CGame2D * g2d, float x, float y, 
					const D3DXMATRIXA16* mat = NULL, 
					D3DCOLOR color = 0xFFFFFFFF, 
					bool Mask = false) const;
	void  Init();
	RECT &GetFrame(int i) { return m_rects[i]; };
	//ִ�ж�������
	void  Update(float fElapsedTime);
	CAnimation2D(CImage2D * img, int clip_w, int clip_h, float delay = 0.1f);
	~CAnimation2D(void);
};
