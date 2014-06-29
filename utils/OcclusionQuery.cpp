#include "OcclusionQuery.h"


COcclusionQuery::COcclusionQuery()
{
    m_query = NULL;
}

COcclusionQuery::~COcclusionQuery()
{
    if (m_query)
    {
        m_query->Release();
        m_query = NULL;
    }
}

bool COcclusionQuery::CreateQuery( LPDIRECT3DDEVICE9 device )
{
    if (FAILED(device->CreateQuery(D3DQUERYTYPE_OCCLUSION, &m_query)))
    {
        MessageBox(NULL, "Create Occlusion Query Error", "ERROR", 0);
        return false;
    }
    return true;
}

bool COcclusionQuery::Begin()
{
    return SUCCEEDED(m_query->Issue(D3DISSUE_BEGIN));
}

bool COcclusionQuery::End()
{
    return SUCCEEDED(m_query->Issue(D3DISSUE_END));
}

DWORD COcclusionQuery::GetResult()
{
    DWORD result;
    while(S_FALSE == m_query->GetData(&result, 
        sizeof(DWORD), D3DGETDATA_FLUSH));
    return result;

}