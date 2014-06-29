/************************************************************************/
/*  Direct3D GameEngine													*/
/*  Image2D 管理器,带缓存                 编写： 姚春晖                 */
/************************************************************************/

#pragma once

#include <map>
#include <string>
#include <d3dx9.h>

class CImage2D;

typedef std::map<std::string, CImage2D*> ImageDict;

class CImage2DManager
{
private:
	ImageDict m_images;
public:
	CImage2D*   AddImage(LPDIRECT3DDEVICE9 device, const std::string &filename, D3DCOLOR color = 0);
	//添加已创建的图像,如果重名就不加入,并返回已有的图像
	CImage2D*   AddImage(CImage2D * image);

	//将图片从缓存移去
	BOOL		RemoveImage(CImage2D * image);
	BOOL		RemoveImage(const std::string &filename);

	CImage2D*   FindImage(const std::string &filename) const;
	
	void		Clear();

	CImage2DManager(void);
	~CImage2DManager(void);
};
