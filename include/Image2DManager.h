/************************************************************************/
/*  Direct3D GameEngine													*/
/*  Image2D ������,������                 ��д�� Ҧ����                 */
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
	//����Ѵ�����ͼ��,��������Ͳ�����,���������е�ͼ��
	CImage2D*   AddImage(CImage2D * image);

	//��ͼƬ�ӻ�����ȥ
	BOOL		RemoveImage(CImage2D * image);
	BOOL		RemoveImage(const std::string &filename);

	CImage2D*   FindImage(const std::string &filename) const;
	
	void		Clear();

	CImage2DManager(void);
	~CImage2DManager(void);
};
