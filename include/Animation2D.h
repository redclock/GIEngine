/************************************************************************/
/* Direct3D GameEngine                                                  */
/* 切分图片的2D动画       编写： 姚春晖                                 */
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
	//每一帧的位置
	vector<RECT> m_rects;
	//计时器
	float m_counter;
public:
	//图像
	CImage2D * Image;
	//每帧的大小
	int	  ClipWidth; 
	int   ClipHeight;
	//是否停止播放
	bool  Stopped;
	//是否循环
	bool  Looped;
	//每帧的延时
	float DelayPerFrame;
	//当前的帧号
	int   CurIndex;
	//动画开始帧号
	int   BeginFrame;
	//动画结束帧号
	int   EndFrame;

	void  Draw(CGame2D * g2d, float x, float y, 
					const D3DXMATRIXA16* mat = NULL, 
					D3DCOLOR color = 0xFFFFFFFF, 
					bool Mask = false) const;
	void  Init();
	RECT &GetFrame(int i) { return m_rects[i]; };
	//执行动画过程
	void  Update(float fElapsedTime);
	CAnimation2D(CImage2D * img, int clip_w, int clip_h, float delay = 0.1f);
	~CAnimation2D(void);
};
