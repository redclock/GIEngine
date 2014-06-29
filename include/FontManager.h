/************************************************************************/
/* Direct3D GameEngine                                                  */
/* ���������,������               ��д�� Ҧ����                        */
/************************************************************************/

#pragma once

#include <vector>
#include <string>
#include <d3dx9.h>


class CFontManager
{
private:
	//��¼������
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
