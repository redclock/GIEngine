#pragma once
#include <d3dx9.h>

class COcclusionQuery
{
private:
    LPDIRECT3DQUERY9 m_query;
    
public:
    COcclusionQuery();
    ~COcclusionQuery();
    bool CreateQuery(LPDIRECT3DDEVICE9 device);
    bool Begin();
    bool End();
    DWORD GetResult();
};