#include "FontManager.h"
#include "GameEngine.h"
#include "GameCommon.h"

CFontManager::CFontManager()
{

}

CFontManager::~CFontManager()
{
	Clear();
}

LPD3DXFONT CFontManager::AddFont(const char * name, UINT size, UINT weight, bool italic)
{
	for (UINT i = 0; i < m_fonts.size(); i ++)
	{
		if (strcmp(name, m_fonts[i]->name) == 0 
			&& size == m_fonts[i]->size 
			&& weight == m_fonts[i]->weight 
			&& italic == m_fonts[i]->italic)
			return m_fonts[i]->font;
	}

	FontItem * f = new FontItem;
	strcpy(f->name, name);
	f->size = size;
	f->italic = italic;
	f->weight = weight;
	D3DXCreateFont(GetDevice(), size, 0, weight, 1, italic, 
					DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 
					DEFAULT_PITCH | FF_DONTCARE, name, & f->font);

	m_fonts.push_back(f);

	return f->font;
}

HRESULT CFontManager::OnLostDevice()
{
	for (UINT i = 0; i < m_fonts.size(); i++)
	{
		if (m_fonts[i]->font)
			m_fonts[i]->font->OnLostDevice();
	}
	return S_OK;
}

HRESULT CFontManager::OnResetDevice()
{
	for (UINT i = 0; i < m_fonts.size(); i++)
	{
		if (m_fonts[i]->font)
			m_fonts[i]->font->OnResetDevice();
	}
	return S_OK;
}

void CFontManager::Clear()
{
	for (UINT i = 0; i < m_fonts.size(); i++)
	{
		SAFE_RELEASE(m_fonts[i]->font);
		SAFE_DELETE(m_fonts[i]);
	}
	m_fonts.clear();
}