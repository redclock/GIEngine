/************************************************************************/
/*  Direct3D GameEngine													*/
/*  Image2D 为2D图像，由Texture实现       编写： 姚春晖                 */
/************************************************************************/

#pragma once

#include <d3dx9.h>
#include <string>

using namespace std;

//CImage2D
class CImage2D 
{
protected:
	//图像信息
	D3DXIMAGE_INFO m_info;
	//图像文件名
	string m_filename;
	//颜色键
	D3DCOLOR m_colorkey;
	//储存图像的贴图
	LPDIRECT3DTEXTURE9 m_pTexture;
	//储存掩位图的贴图
	LPDIRECT3DTEXTURE9 m_ptxtMask;
public: 
	//从文件中装载, color表示颜色键
	bool LoadFromFile(LPDIRECT3DDEVICE9 device, const string &filename, D3DCOLOR color = 0xFF000000);
	//重新装载
	virtual bool ReLoad(LPDIRECT3DDEVICE9 device);
	//是否已经成功装载
	bool IsAvailable() const { return m_pTexture != NULL; }; 
	
	//--得到相应信息
	D3DCOLOR	GetColorKey() const { return m_colorkey; } 
	string		GetFileName() const { return m_filename; }; 
	int			GetWidth() const { return m_info.Width; };
	int			GetHeight() const { return m_info.Height; };
	D3DFORMAT	GetFormat() const { return m_info.Format; };
	UINT		GetDepth() const { return m_info.Depth; };
	D3DXIMAGE_FILEFORMAT GetImageFileFormat() const { return m_info.ImageFileFormat; };
	//--
	
	//得到贴图
	LPDIRECT3DTEXTURE9 GetTexture() const { return m_pTexture; };

	//得到掩位图
	LPDIRECT3DTEXTURE9  GetMaskTexture(LPDIRECT3DDEVICE9 device);
	
	//销毁贴图
	virtual void Destroy();
	
	//贴图大小,补齐到2的n次方
	UINT TextureWidth;
	UINT TextureHeight;

	CImage2D(void);
	virtual ~CImage2D(void);
};
