/************************************************************************/
/* Direct3D GameEngine                                                  */
/* 2D渲染引擎                      编写： 姚春晖                        */
/************************************************************************/

#pragma once

#include "Image2D.h"


class CGame2D
{
private:
	LPDIRECT3DDEVICE9 m_device;
	LPD3DXSPRITE	m_sprite;
	//是否在Sprite的Begin和End之间
	bool			m_active;
public:
	
	CGame2D();
	~CGame2D();

	void        OnLostDevice();
	HRESULT     OnResetDevice();

	HRESULT		Init(LPDIRECT3DDEVICE9 device);

	HRESULT     BeginDraw();

	void		DrawImage(CImage2D* img, float x, float y, const RECT * rect = NULL, const D3DXMATRIXA16* mat = NULL, D3DCOLOR color = 0xFFFFFFFF, bool Mask = false);
	void		DrawImageScale(CImage2D* img, float x, float y, float w, float h, const RECT * rect = NULL, D3DCOLOR color = 0xFFFFFFFF, bool Mask = false);

	void		DrawTexture(LPDIRECT3DTEXTURE9 tex, float x, float y, float w, float h, const RECT * rect = NULL, const D3DXMATRIXA16* pmat = NULL, D3DCOLOR color = 0xFFFFFFFF);
	
	void		DrawString(LPD3DXFONT font, const char * s, int x, int y, D3DCOLOR color, DWORD format = DT_LEFT, const D3DXMATRIXA16* pmat = NULL);
	void		DrawString(LPD3DXFONT font, const char * s, RECT * rt, D3DCOLOR color, DWORD format = DT_CENTER, const D3DXMATRIXA16* pmat = NULL);

	void		DrawStringShadow(LPD3DXFONT font, const char * s, int x, int y, D3DCOLOR color, D3DCOLOR shadow = 0xFF000000, DWORD format = DT_LEFT, const D3DXMATRIXA16* pmat = NULL);
	void		DrawStringShadow(LPD3DXFONT font, const char * s, RECT * rt, D3DCOLOR color, D3DCOLOR shadow = 0xFF000000, DWORD format = DT_CENTER, const D3DXMATRIXA16* pmat = NULL);

	HRESULT		Flush();
	HRESULT		EndDraw();

	HRESULT		DrawRect(const RECT* pRect, D3DCOLOR color );

    HRESULT		DrawRect(int x, int y, int w, int h, D3DCOLOR color);

    HRESULT		DrawLine(float x1, float y1, float x2, float y2, D3DCOLOR color);

	HRESULT		ScreenShot(const char * filename);

	bool		IsActive() const { return m_active; };

};