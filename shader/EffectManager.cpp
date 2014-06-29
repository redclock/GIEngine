#include "EffectManager.h"
#include "MyLib.h"
#include "GameCommon.h"
#include "Log.h"
#include <assert.h>

CEffectManager::CEffectManager(void)
{
}

CEffectManager::~CEffectManager(void)
{
    Clear();
}

LPD3DXEFFECT CEffectManager::FindEffect(const std::string &filename) const
{
    EffectDict::const_iterator itr = m_effects.find(filename);
    if (itr == m_effects.end()) return NULL;
    return itr->second;
}

CEffectManager::EffectDict::iterator CEffectManager::FindEffect(const LPD3DXEFFECT effect)
{
    EffectDict::iterator itr = m_effects.begin();
    while (itr != m_effects.end() && itr->second != effect)
        itr++;
    return itr;
}

LPD3DXEFFECT CEffectManager::LoadEffect( LPDIRECT3DDEVICE9 device, const std::string &filename )
{
    LPD3DXEFFECT effect = FindEffect(filename);
    if (effect)
        return effect;
    effect = CreateEffectFromFile(filename.c_str());
    GetLog() << CLog::TYPE_GRAPH << "Load Effect file: " << filename;
    if (effect)
    {
        m_effects[filename] = effect;
        return effect;
    }
    else
    {
        GetLog() << CLog::TYPE_ERROR << "File not found: " << filename;
        return NULL;
    }

}

BOOL CEffectManager::UnloadEffect( const LPD3DXEFFECT effect )
{
    EffectDict::iterator itr = FindEffect(effect);
    if (itr == m_effects.end())
        return FALSE;
    LPD3DXEFFECT e = itr->second;
    assert(effect == e);
    SAFE_RELEASE(e);
    m_effects.erase(itr);
    return TRUE;
}

BOOL CEffectManager::RemoveEffect( const LPD3DXEFFECT effect )
{
    EffectDict::iterator itr = FindEffect(effect);
    if (itr == m_effects.end())
        return FALSE;
    m_effects.erase(itr);
    return TRUE;
}

void CEffectManager::Clear()
{
    EffectDict::iterator itr = m_effects.begin();
    while (itr != m_effects.end())
    {
        SAFE_RELEASE(itr->second);
        itr++;
    }
    m_effects.clear();
}

HRESULT CEffectManager::OnLostDevice()
{
    EffectDict::iterator itr = m_effects.begin();
    while (itr != m_effects.end())
    {
        LPD3DXEFFECT e = itr->second;
        if (e) e->OnLostDevice();
        itr++;
    }
    return S_OK;

}

HRESULT CEffectManager::OnResetDevice()
{
    EffectDict::iterator itr = m_effects.begin();
    while (itr != m_effects.end())
    {
        LPD3DXEFFECT e = itr->second;
        if (e) e->OnResetDevice();
        itr++;
    }
    return S_OK;

}