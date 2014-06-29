#include <assert.h>
#include "Animation2D.h"
#include "Game2D.h"

CAnimation2D::CAnimation2D(CImage2D * img, int clip_w, int clip_h, float delay)
{
	Image			= img;
	ClipWidth		= clip_w;
	ClipHeight		= clip_h;
	DelayPerFrame	= delay;
	BeginFrame		= -1;	
	EndFrame		= -1;
	CurIndex		= 0;
	Stopped			= false;
	Looped			= true;
	Init();
}

CAnimation2D::~CAnimation2D(void)
{
}

void CAnimation2D::Init()
{
	assert(Image != NULL && ClipWidth > 0 && ClipHeight > 0);
	//make clips 
	int iw = Image->GetWidth();
	int ih = Image->GetHeight();
	//一行格子数
	int nx = (iw + ClipWidth - 1)/ ClipWidth;
	//一列格子数
	int ny = (ih + ClipHeight - 1)/ ClipHeight;
	
	//根据贴图实际大小缩放
	float cw = ClipWidth * Image->TextureWidth  / (float) Image->GetWidth();
	float ch = ClipHeight * Image->TextureHeight / (float) Image->GetHeight();

	//总格子数
	int count = nx * ny;
	m_rects.clear();

	for (int i = 0; i < count; i++)
	{
		RECT rt;
		rt.left = int (i % nx * cw + 0.5f);
		rt.top =  int (i / nx * ch + 0.5f);
		rt.right = rt.left + int (cw + 0.5f);
		rt.bottom = rt.top + int (ch + 0.5f);
		m_rects.push_back(rt);
	}
	if (BeginFrame < 0) BeginFrame = 0;
	if (EndFrame < 0) EndFrame = count - 1;
	if (CurIndex < BeginFrame) CurIndex = BeginFrame;
	if (CurIndex > EndFrame) CurIndex = EndFrame;
	m_counter = 0;
}

void CAnimation2D::Update(float fElapsedTime)
{
	if (Stopped) return;
	//计时
	m_counter += fElapsedTime;
	//超过的时间
	while (m_counter >= DelayPerFrame)
	{
		//跳到下一帧
		if (CurIndex >= EndFrame)
		{
			if (Looped)	
				CurIndex = 0;
			else 
				break;
		}
		else 
			CurIndex ++;
		m_counter -= DelayPerFrame;
	}
}

void CAnimation2D::Draw(CGame2D * g2d, float x, float y, const D3DXMATRIXA16* mat, D3DCOLOR color, bool Mask) const
{
	assert(Image != NULL);
	if (CurIndex < 0 || CurIndex >= (int) m_rects.size()) return;
	RECT rt = m_rects[CurIndex]; 
	g2d->DrawImage(Image, x, y, &rt, mat, color, Mask);
}