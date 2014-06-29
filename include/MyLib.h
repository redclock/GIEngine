/************************************************************************/
/*  Direct3D GameEngine													*/
/*  实用函数                              编写： 姚春晖                 */
/************************************************************************/

#pragma once

#include <D3DX9.h>
#include <d3d9types.h>

const int fxCopy   = 1;        //原样
const int fxBlend  = 2;        //混合
const int fxAdd    = 4;        //叠加
const int fxSub    = 8;        //相减
const int fxMult   = 16;       //相乘


// vertex formats

struct VERTEX_XYZC
{
	FLOAT x, y, z;
	DWORD color;
	static const DWORD FVF;
};


struct VERTEX_XYZRHWC
{
	FLOAT x, y, z;
	FLOAT rhw;
	DWORD color;
	static const DWORD FVF;
};


struct VERTEX_XYZRHWTEX1
{
	FLOAT x, y, z;
	FLOAT rhw;
	DWORD color;
	FLOAT tu, tv;   
	static const DWORD FVF;
};

struct VERTEX_XYZNORM
{
	FLOAT x, y, z;
	FLOAT nx, ny, nz;
	DWORD color;
	static const DWORD FVF;
};

struct VERTEX_TEX1
{
	FLOAT x, y, z;
	DWORD color;
	FLOAT  tu, tv;   
	static const DWORD FVF;
};

struct VERTEX_TEX2
{
	FLOAT x, y, z;
	DWORD color;
	FLOAT  tu1, tv1; 
	FLOAT  tu2, tv2; 
	static const DWORD FVF;
};

void TextureModulate(LPDIRECT3DTEXTURE9 Tex1, LPDIRECT3DTEXTURE9 Tex2);
void TextureSelect(LPDIRECT3DTEXTURE9 Tex1);
void TextureColor(DWORD color);
void TextureNone();
void TextureAmbient(LPDIRECT3DTEXTURE9 Tex1);
void TextureModulateDiffuse(LPDIRECT3DTEXTURE9 Tex1);
void TextureModulateColor(LPDIRECT3DTEXTURE9 Tex1, DWORD color);
void TextureAdd(LPDIRECT3DTEXTURE9 Tex1, LPDIRECT3DTEXTURE9 Tex2);
void TextureDiffuseBlend(LPDIRECT3DTEXTURE9 Tex1, LPDIRECT3DTEXTURE9 Tex2);

void BeginAlpha();
void EndAlpha();

void AlphaSrcOnly();
void AlphaDestOnly();
void AlphaAdd();
void AlphaSrcColor();
void AlphaDestColor();
void AlphaSrcAlpha();
void AlphaDestAlpha();
void AlphaMultiply();
void AlphaMultiply2();

void SetEffect(int DrawFx, bool force = false);

LPD3DXEFFECT CreateEffectFromFile(const char* filename);

void GammaFade(float r, float g, float b);

void VisualizeTextureLevel(LPDIRECT3DTEXTURE9 tex, int level, int x, int y, int w, int h, float gamma = 1.0f);

void MaskTextureLevel(LPDIRECT3DTEXTURE9 tex, int level, int x, int y, int w, int h,
                      float minMask, float maxMask, int maskValue);

D3DXMATRIX GetCubeMapViewMatrix(float ex, float ey, float ez, DWORD dwFace);

//检查显卡能力
HRESULT CheckResourceFormatSupport(LPDIRECT3DDEVICE9 device, D3DFORMAT fmt, 
                                   D3DRESOURCETYPE resType, DWORD dwUsage);

inline float RandomRange(float min, float max)
{
	return rand() * ( max - min ) / (RAND_MAX + 1.0F) + min;
}

inline int GetRectWidth(const RECT &rt)
{
	return rt.right - rt.left;
}

inline int GetRectHeight(const RECT &rt)
{
	return rt.bottom - rt.top;
}

inline RECT MakeRect(int x1, int y1, int x2, int y2)
{
	RECT rt;
	rt.left = x1;
	rt.right = x2;
	rt.top = y1;
	rt.bottom = y2;
	return rt;
}

inline RECT MakeBounds(int x, int y, int w, int h)
{
	return MakeRect(x, y, x + w, y + h);
}

inline void MoveRect(RECT & rt, int destx, int desty)
{ 
	int dx = destx - rt.left;
	int dy = desty - rt.top;
	rt.left = destx;
	rt.top  = desty;
	rt.right += dx;
	rt.bottom += dy;
}

//使角度范围：－PI~PI
float NormAngle(float angle);

float GetAngle(float x, float y);

inline DWORD FtoDW(float f)
{ 
	return *((DWORD*)&f); 
}


inline float GetLineLengthSqr(float x1, float y1, float x2, float y2)
{
	return ((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2)); 
}


inline DWORD ColorAdd(DWORD c1, DWORD c2)
{
	DWORD r = GetRValue(c1) + GetRValue(c2);
	DWORD g = GetGValue(c1) + GetGValue(c2);
	DWORD b = GetBValue(c1) + GetBValue(c2);

	return RGB(min(r, 255), min(g, 255), min(b, 255)) | 0xFF000000;
}

inline DWORD ColorScale(DWORD c, float k)
{
	return RGB(min((int)(GetRValue(c) * k), 255),
		min((int)(GetGValue(c) * k), 255),
		min((int)(GetBValue(c) * k), 255)) | 0xFF000000;
}

void ScreenToDir(int x, int y, float* dx, float* dy, float* dz);

#define DX_BEGIN_EFFECT(effect)  {                                      \
                    UINT cPasses;                                       \
                    if (SUCCEEDED(effect->Begin(&cPasses, 0)))          \
                    {                                                   \
                        for (UINT iPass = 0; iPass < cPasses; iPass++)  \
                        {                                               \
                            if (SUCCEEDED(effect->BeginPass(iPass)))    \
                            {                                           

#define DX_END_EFFECT(effect)   effect->EndPass();                      \
                            }                                           \
                        }                                               \
                        effect->End();                                  \
                    }                                                   \
                }   
