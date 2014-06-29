/************************************************************************/
/*  Direct3D GameEngine													*/
/*  Image2D Ϊ2Dͼ����Textureʵ��       ��д�� Ҧ����                 */
/************************************************************************/

#pragma once

#include <d3dx9.h>
#include <string>

using namespace std;

//CImage2D
class CImage2D 
{
protected:
	//ͼ����Ϣ
	D3DXIMAGE_INFO m_info;
	//ͼ���ļ���
	string m_filename;
	//��ɫ��
	D3DCOLOR m_colorkey;
	//����ͼ�����ͼ
	LPDIRECT3DTEXTURE9 m_pTexture;
	//������λͼ����ͼ
	LPDIRECT3DTEXTURE9 m_ptxtMask;
public: 
	//���ļ���װ��, color��ʾ��ɫ��
	bool LoadFromFile(LPDIRECT3DDEVICE9 device, const string &filename, D3DCOLOR color = 0xFF000000);
	//����װ��
	virtual bool ReLoad(LPDIRECT3DDEVICE9 device);
	//�Ƿ��Ѿ��ɹ�װ��
	bool IsAvailable() const { return m_pTexture != NULL; }; 
	
	//--�õ���Ӧ��Ϣ
	D3DCOLOR	GetColorKey() const { return m_colorkey; } 
	string		GetFileName() const { return m_filename; }; 
	int			GetWidth() const { return m_info.Width; };
	int			GetHeight() const { return m_info.Height; };
	D3DFORMAT	GetFormat() const { return m_info.Format; };
	UINT		GetDepth() const { return m_info.Depth; };
	D3DXIMAGE_FILEFORMAT GetImageFileFormat() const { return m_info.ImageFileFormat; };
	//--
	
	//�õ���ͼ
	LPDIRECT3DTEXTURE9 GetTexture() const { return m_pTexture; };

	//�õ���λͼ
	LPDIRECT3DTEXTURE9  GetMaskTexture(LPDIRECT3DDEVICE9 device);
	
	//������ͼ
	virtual void Destroy();
	
	//��ͼ��С,���뵽2��n�η�
	UINT TextureWidth;
	UINT TextureHeight;

	CImage2D(void);
	virtual ~CImage2D(void);
};
