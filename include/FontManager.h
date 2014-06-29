/************************************************************************/
/* Direct3D GameEngine                                                  */
/* 字体管理器,带缓存               编写： 姚春晖                        */
/************************************************************************/

#pragma once

#include <vector>
#include <string>
#include <d3dx9.h>


class CFontManager
{
private:
	//记录字体项
	struct FontItem
	{
		char name[100];
		UINT size;
		UINT weight;
		bool italic;
		LPD3DXFONT font;
	};

	std::vector<FontItem *> m_fonts;
public:
	LPD3DXFONT	AddFont(const char * name, UINT size, UINT weight = FW_NORMAL, bool italic = false);
	HRESULT		OnLostDevice();
	HRESULT		OnResetDevice();
	void		Clear();

	CFontManager(void);
	~CFontManager(void);
};
