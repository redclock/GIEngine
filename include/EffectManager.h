#pragma once

#include <map>
#include <string>
#include <d3dx9.h>


class CEffectManager
{
private:
    typedef std::map<std::string, LPD3DXEFFECT> EffectDict;
    EffectDict m_effects;
    EffectDict::iterator FindEffect(const LPD3DXEFFECT effect);

public:
    LPD3DXEFFECT LoadEffect(LPDIRECT3DDEVICE9 device, const std::string &filename);
    BOOL UnloadEffect(const LPD3DXEFFECT effect);
    BOOL UnloadEffect(const std::string &filename);
    BOOL RemoveEffect(const LPD3DXEFFECT effect);
    LPD3DXEFFECT FindEffect(const std::string &filename) const;
    void Clear();
    HRESULT OnLostDevice();
    HRESULT OnResetDevice();

    CEffectManager(void);
    ~CEffectManager(void);
};