#include <assert.h>
#include "Image2D.h"
#include "GameCommon.h"

//////////////////////////////////
// CImage2D��ʵ��
//////////////////////////////////

//��ʼ��
CImage2D::CImage2D(void)
{
	m_filename = "";
	m_pTexture = NULL;
	m_ptxtMask = NULL;
}

//������ͼ
CImage2D::~CImage2D(void)
{
	SAFE_RELEASE(m_pTexture);
	SAFE_RELEASE(m_ptxtMask);
}


//���ļ���װ��
bool CImage2D::LoadFromFile(LPDIRECT3DDEVICE9 device, const string & filename, D3DCOLOR color)
{
	assert(device != NULL);
	
	// Create texture
	HRESULT hr = D3DXCreateTextureFromFileEx(
					device, 
					filename.c_str(), 
					0, 0, D3DX_FROM_FILE, 0 , 
					D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, 
					D3DX_DEFAULT, D3DX_DEFAULT, 
					color,
					&m_info, NULL, &m_pTexture);

	if (FAILED(hr))
	{
		return false;
	}

	m_filename = filename;
	m_colorkey = color;
	//�õ���ͼ��С,Ϊ2��n�η�
	D3DSURFACE_DESC ddsd;
	m_pTexture->GetLevelDesc(0, &ddsd);
	TextureWidth = ddsd.Width;
	TextureHeight = ddsd.Height;

	m_ptxtMask = NULL;

	return true;
}

bool CImage2D::ReLoad(LPDIRECT3DDEVICE9 device)
{
	return LoadFromFile(device, m_filename, m_colorkey);
}

void CImage2D::Destroy()
{
	SAFE_RELEASE(m_pTexture);
	SAFE_RELEASE(m_ptxtMask);
	
}


//�õ���ģͼ,û�оͽ���һ��
LPDIRECT3DTEXTURE9 CImage2D::GetMaskTexture(LPDIRECT3DDEVICE9 device)
{
	assert(m_pTexture != NULL);
	//�Ѿ�����,ֱ�ӷ���
	if (m_ptxtMask) return m_ptxtMask;
	
	//������
	HRESULT hr = device->CreateTexture(TextureWidth, TextureHeight, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_ptxtMask, NULL);
	if (FAILED(hr)) return NULL;
	
	//������ͼ
	D3DLOCKED_RECT rt_mask;
	D3DLOCKED_RECT rt_txt;

	m_ptxtMask->LockRect(0, &rt_mask, NULL, D3DLOCK_DISCARD);
	m_pTexture->LockRect(0, &rt_txt, NULL, D3DLOCK_READONLY);
	DWORD * line_mask = (DWORD *) rt_mask.pBits;
	DWORD * line_txt = (DWORD *) rt_txt.pBits;
	for(UINT row = 0; row < TextureHeight; row ++)
	{
		for (UINT col = 0; col < TextureWidth; col ++)
		{
			DWORD Alpha = (line_txt[col])>> 24;
			line_mask[col] = (Alpha << 24) | (Alpha << 16) | (Alpha << 8) | Alpha;
		}
		line_mask += rt_mask.Pitch >> 2;
		line_txt  += rt_txt.Pitch >> 2;
	}

	m_pTexture->UnlockRect(0);
	m_ptxtMask->UnlockRect(0);
	return m_ptxtMask;
}

