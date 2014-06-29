#include "Fog.h"
#include "GameEngine.h"
#include "MyLib.h"

void CFog::SetFogMode(D3DFOGMODE mode)
{
	GetDevice()->SetRenderState(D3DRS_FOGTABLEMODE, mode);
}

void CFog::SetStart(float v)
{
	GetDevice()->SetRenderState(D3DRS_FOGSTART, FtoDW(v));
}

void CFog::SetEnd(float v)
{
	GetDevice()->SetRenderState(D3DRS_FOGEND, FtoDW(v));
}

void CFog::SetDensity(float v)
{
	GetDevice()->SetRenderState(D3DRS_FOGDENSITY, FtoDW(v));
}

void CFog::SetColor(DWORD color)
{
	GetDevice()->SetRenderState(D3DRS_FOGCOLOR, color);
}

void CFog::Enable(BOOL v)
{
	GetDevice()->SetRenderState(D3DRS_FOGENABLE, v);
}