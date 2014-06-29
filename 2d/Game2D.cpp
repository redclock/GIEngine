#include "Game2D.h"
#include <assert.h>


#include "MyLib.h"
#include "GameCommon.h"

D3DXMATRIX IdentityMatrix(1, 0, 0, 0,
						0, 1, 0, 0,
						0, 0, 1, 0,
						0, 0, 0, 1);

CGame2D::CGame2D()
{
	m_sprite = NULL;
	m_active = false;
}

CGame2D::~CGame2D()
{
	SAFE_RELEASE(m_sprite);
}

HRESULT CGame2D::Init(LPDIRECT3DDEVICE9 device)
{
	m_device = device;
	HRESULT hr = D3DXCreateSprite( m_device, &m_sprite );
	return hr;
}

void CGame2D::DrawImage(CImage2D* img, float x, float y, const RECT * rect, const D3DXMATRIXA16* pmat, D3DCOLOR color, bool Mask)
{
	assert(m_sprite && img != NULL && img->IsAvailable());
	if (img->GetWidth() == 0 || img->GetHeight() == 0) return;
	float scaleX, scaleY;
	scaleX = (float) img->GetWidth()  / (float) img->TextureWidth;	
	scaleY = (float) img->GetHeight() / (float) img->TextureHeight;	
	D3DXMATRIXA16 matTransform;
	D3DXMatrixScaling(&matTransform, scaleX, scaleY, 1.0f);
	if (pmat != NULL)
	{
		matTransform *= (* pmat);
	}
	D3DXMATRIXA16 mPos;
	D3DXMatrixTranslation(&mPos, x, y, 0);
	matTransform = matTransform * mPos;
	m_sprite->SetTransform( &matTransform );
	HRESULT hr;
	if (Mask)
	{
		LPDIRECT3DTEXTURE9 m = img->GetMaskTexture(m_device);
		if (m)
			if (FAILED(hr = m_sprite->Draw(img->GetMaskTexture(m_device), rect, NULL, NULL, color)))
				return;
	}
	else
	{
		if (FAILED(hr = m_sprite->Draw(img->GetTexture(), rect, NULL, NULL, color)))
			return;
	}
}

void CGame2D::DrawTexture(LPDIRECT3DTEXTURE9 tex, float x, float y, float w, float h, const RECT * rect, const D3DXMATRIXA16* pmat, D3DCOLOR color)
{
	assert(m_sprite && tex != NULL);
	if (w <= 0 || h <= 0) return;
	float scaleX, scaleY;
	D3DSURFACE_DESC ddsd;
	tex->GetLevelDesc(0, &ddsd);
	scaleX = w  / ddsd.Width;	
	scaleY = h  / ddsd.Height;	
	D3DXMATRIXA16 matTransform;
	D3DXMatrixScaling(&matTransform, scaleX, scaleY, 1.0f);
	//D3DXMatrixIdentity(&matTransform);
	if (pmat != NULL)
	{
		matTransform *= (* pmat);
	}
	D3DXMATRIXA16 mPos;
	D3DXMatrixTranslation(&mPos, x - 0, y - 0, 0);
	matTransform = matTransform * mPos;
	m_sprite->SetTransform( &matTransform );
	HRESULT hr;
	if (FAILED(hr = m_sprite->Draw(tex, rect, NULL, NULL, color)))
		return;
}

void CGame2D::DrawImageScale(CImage2D* img, float x, float y, float w, float h, const RECT * rect, D3DCOLOR color, bool Mask)
{
	if (img != NULL)
	{
		D3DXMATRIXA16 mat;
		int iw = img->GetWidth();
		int ih = img->GetHeight();
		D3DXMatrixScaling(&mat, w /(float)iw, h / (float)ih, 1.0f);
		DrawImage(img, x, y, rect, &mat, color, Mask);
	}
}

void CGame2D::DrawString(LPD3DXFONT font, const char * s, RECT * rt, D3DCOLOR color, DWORD format, const D3DXMATRIXA16* pmat)
{
	m_sprite->SetTransform(& IdentityMatrix);
	font->DrawText(m_sprite, s, -1, rt, format, color);
}

void CGame2D::DrawString(LPD3DXFONT font, const char * s, int x, int y, D3DCOLOR color, DWORD format, const D3DXMATRIXA16* pmat)
{
	m_sprite->SetTransform(& IdentityMatrix);
	RECT rt = {x, y, x, y};
	font->DrawText(m_sprite, s, -1, &rt, format | DT_NOCLIP, color);
}

void CGame2D::DrawStringShadow(LPD3DXFONT font, const char * s, int x, int y, D3DCOLOR color, D3DCOLOR shadow, DWORD format, const D3DXMATRIXA16* pmat)
{
	DrawString(font, s, x + 1, y + 1, shadow, format, pmat);
	DrawString(font, s, x, y, color, format, pmat);
}

void CGame2D::DrawStringShadow(LPD3DXFONT font, const char * s, RECT * rt, D3DCOLOR color, D3DCOLOR shadow, DWORD format, const D3DXMATRIXA16* pmat)
{
	RECT rtShadow = {rt->left + 1, rt->top + 1, rt->right + 1, rt->bottom + 1};
	DrawString(font, s, &rtShadow, shadow, format, pmat);
	DrawString(font, s, rt, color, format, pmat);
}

HRESULT CGame2D::BeginDraw()
{
	m_active = true;
	return m_sprite->Begin( NULL );
}

HRESULT CGame2D::EndDraw()
{
	m_active = false;
	return m_sprite->End();
}

HRESULT	CGame2D::Flush()
{
	return m_sprite->Flush();
}

void CGame2D::OnLostDevice()
{
	if (m_sprite) m_sprite->OnLostDevice();	
}

HRESULT CGame2D::OnResetDevice()
{
	if (m_sprite) m_sprite->OnResetDevice();
	return S_OK;
}

HRESULT CGame2D::DrawRect(const RECT* pRect, D3DCOLOR color )
{
	
	//没有半透明时
	if ((color & 0xFF000000) == 0xFF000000) 
	{
		LPDIRECT3DSURFACE9 buf;
		m_device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &buf);
		if (buf)
		{
			m_device->ColorFill(buf, pRect, color);
			SAFE_RELEASE(buf);
		}
		return S_OK;
	}
	RECT rcScreen = *pRect;

	VERTEX_XYZRHWTEX1 vertices[4] =
	{
		(float) rcScreen.left -0.5f,  (float) rcScreen.top -0.5f,    0.5f, 1.0f, color, 0, 0,
		(float) rcScreen.right -0.5f, (float) rcScreen.top -0.5f,    0.5f, 1.0f, color, 1, 0, 
		(float) rcScreen.right -0.5f, (float) rcScreen.bottom -0.5f, 0.5f, 1.0f, color, 1, 1, 
		(float) rcScreen.left -0.5f,  (float) rcScreen.bottom -0.5f, 0.5f, 1.0f, color, 0, 1,
	};

	m_device->SetFVF( VERTEX_XYZRHWTEX1::FVF );

	m_device->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, vertices, sizeof(VERTEX_XYZRHWTEX1) );

	return S_OK;
}

HRESULT CGame2D::DrawRect(int x, int y, int w, int h, D3DCOLOR color)
{

    VERTEX_XYZRHWTEX1 vertices[4] =
    {
        (float) x - 0.5f,  (float)y -0.5f,  0.5f, 1.0f, color, 0, 0,
        (float) x + w - 0.5f, (float) y - 0.5f,    0.5f, 1.0f, color, 1, 0, 
        (float) x + w - 0.5f, (float) y + h - 0.5f, 0.5f, 1.0f, color, 1, 1, 
        (float) x - 0.5f,  (float) y + h - 0.5f, 0.5f, 1.0f, color, 0, 1,
    };

    m_device->SetFVF( VERTEX_XYZRHWTEX1::FVF );

    return m_device->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, vertices, sizeof(VERTEX_XYZRHWTEX1) );
}

HRESULT CGame2D::ScreenShot(const char * filename)
{
	HRESULT hr;
	LPDIRECT3DSURFACE9 buf;

	if (FAILED( hr = m_device->GetRenderTarget(0, &buf))) 
		return hr;
	hr = D3DXSaveSurfaceToFile(filename, D3DXIFF_PNG, buf, NULL, NULL);
	SAFE_RELEASE(buf);
	return hr;
}

HRESULT CGame2D::DrawLine(float x1, float y1, float x2, float y2, D3DCOLOR color)
{
    VERTEX_XYZRHWC vertices[2] =
    {
        x1,  y1, 0.5f, 1.0f, color,
        x2,  y2, 0.5f, 1.0f, color,
    };

    m_device->SetFVF( VERTEX_XYZRHWC::FVF );

    return m_device->DrawPrimitiveUP( D3DPT_LINELIST, 1, vertices, sizeof(VERTEX_XYZRHWC) );
}