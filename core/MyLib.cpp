#include "MyLib.h"
#include "GameEngine.h"

const DWORD VERTEX_XYZC::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;
const DWORD VERTEX_XYZRHWC::FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE;
const DWORD VERTEX_XYZRHWTEX1::FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;
const DWORD VERTEX_XYZNORM::FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE;
const DWORD VERTEX_TEX1::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1;
const DWORD VERTEX_TEX2::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX2;

int curEffect = -1;

void TextureModulate(LPDIRECT3DTEXTURE9 Tex1, LPDIRECT3DTEXTURE9 Tex2)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	pDevice->SetTexture(0, Tex1);
	pDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0);
	pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );

	pDevice->SetTexture(1, Tex2);
	pDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1);
	pDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE );

	pDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );

}

void TextureModulateDiffuse(LPDIRECT3DTEXTURE9 Tex1)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	pDevice->SetTexture(0, Tex1);
	pDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0);
	pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );

	pDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
}

void TextureModulateColor(LPDIRECT3DTEXTURE9 Tex1, DWORD color)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	pDevice->SetTexture(0, Tex1);
	pDevice->SetRenderState( D3DRS_TEXTUREFACTOR, color);
	pDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0);
	pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR );
	pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR );
	pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );

	pDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
}

void TextureSelect(LPDIRECT3DTEXTURE9 Tex1)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	pDevice->SetTexture(0, Tex1);
	pDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0);
	pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );

	pDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );

}

void TextureNone()
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	pDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0);
	pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
	pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    pDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
}

void TextureColor(DWORD color)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	pDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0);
	pDevice->SetRenderState( D3DRS_TEXTUREFACTOR, color);
	pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TFACTOR );
	pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
}

void TextureAmbient(LPDIRECT3DTEXTURE9 Tex1)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	pDevice->SetTexture(0, Tex1);
	pDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0);
	pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
	pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
}

void TextureAdd(LPDIRECT3DTEXTURE9 Tex1, LPDIRECT3DTEXTURE9 Tex2)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	pDevice->SetTexture(0, Tex1);
	pDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0);
	pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
	pDevice->SetTexture(1, Tex2);
	pDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1);
	pDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
	pDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_ADD );
	pDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
}

void TextureDiffuseBlend(LPDIRECT3DTEXTURE9 Tex1, LPDIRECT3DTEXTURE9 Tex2)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	pDevice->SetTexture(0, Tex1);
	pDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0);
	pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
	pDevice->SetTexture(1, Tex2);
	pDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1);
	pDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
	pDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_BLENDDIFFUSEALPHA  );
	pDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
}

void BeginAlpha()
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
}
void EndAlpha()
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
}

void AlphaSrcOnly()
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	pDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
	pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ZERO );
}

void AlphaMultiply()
{
    LPDIRECT3DDEVICE9 pDevice = GetDevice();
    pDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR );
    pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ZERO );
}

void AlphaMultiply2()
{
    LPDIRECT3DDEVICE9 pDevice = GetDevice();
    pDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR );
    pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR );
}

void AlphaDestOnly()
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	pDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ZERO );
	pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
}

void AlphaAdd()
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	pDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
}

void AlphaSrcColor()
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	pDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_INVSRCCOLOR );
	pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR );
}

void AlphaDestColor()
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	pDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR );
	pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVDESTCOLOR );
}

void AlphaSrcAlpha()
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	pDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

void AlphaDestAlpha()
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	pDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_DESTALPHA );
	pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVDESTALPHA );
}

void GammaFade(float r, float g, float b)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	D3DGAMMARAMP gamma;
	for (int i = 0; i < 256; i++) gamma.blue[i] = int(i * 256 * b);
	for (int i = 0; i < 256; i++) gamma.green[i] = int(i * 256 * g);
	for (int i = 0; i < 256; i++) gamma.red[i] = int(i * 256 * r);
	pDevice->SetGammaRamp(0, D3DSGR_CALIBRATE, &gamma);
}

void SetEffect(int DrawFx, bool force)
{
	if (force || DrawFx != curEffect) 
	{
		if (Get2D()->IsActive()) Get2D()->Flush();
		switch(DrawFx) 
		{
		case fxCopy:
			AlphaSrcOnly();
			break;
		case fxBlend:
			AlphaSrcAlpha();
			break;
		case fxAdd:
			AlphaAdd();
			break;
		case fxSub:
			AlphaSrcOnly();
			break;
        case fxMult:
            AlphaMultiply();
            break;
		default:
			;
		}
	}
	curEffect = DrawFx;
}

LPD3DXEFFECT CreateEffectFromFile(const char* filename)
{
    // Define DEBUG_VS and/or DEBUG_PS to debug vertex and/or pixel shaders with the 
    // shader debugger. Debugging vertex shaders requires either REF or software vertex 
    // processing, and debugging pixel shaders requires REF.  The 
    // D3DXSHADER_FORCE_*_SOFTWARE_NOOPT flag improves the debug experience in the 
    // shader debugger.  It enables source level debugging, prevents instruction 
    // reordering, prevents dead code elimination, and forces the compiler to compile 
    // against the next higher available software target, which ensures that the 
    // unoptimized shaders do not exceed the shader model limitations.  Setting these 
    // flags will cause slower rendering since the shaders will be unoptimized and 
    // forced into software.  See the DirectX documentation for more information about 
    // using the shader debugger.
    DWORD dwShaderFlags = D3DXFX_NOT_CLONEABLE;

#if defined( DEBUG ) || defined( _DEBUG )
    // Set the D3DXSHADER_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DXSHADER_DEBUG;
#endif

#ifdef DEBUG_VS
    dwShaderFlags |= D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT;
#endif
#ifdef DEBUG_PS
    dwShaderFlags |= D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT;
#endif

    // Preshaders are parts of the shader that the effect system pulls out of the 
    // shader and runs on the host CPU. They should be used if you are GPU limited. 
    // The D3DXSHADER_NO_PRESHADER flag disables preshaders.
    bool g_bEnablePreshader = false;
    if( !g_bEnablePreshader )
        dwShaderFlags |= D3DXSHADER_NO_PRESHADER;
    LPD3DXEFFECT effect;
    // If this fails, there should be debug output as to 
    // why the .fx file failed to compile
    if (FAILED(D3DXCreateEffectFromFile(GetDevice(), filename, NULL, NULL, dwShaderFlags, NULL, &effect, NULL)))
    {
        return NULL;
    }
    return effect;

}

float NormAngle(float angle)
{
	while (angle > 2 * D3DX_PI) angle -= 2 * D3DX_PI;
	while (angle < 0) angle += 2 * D3DX_PI;
	if (angle > D3DX_PI) angle -= 2 * D3DX_PI; 
	return angle;
}

float GetAngle(float x, float y)
{
	if (fabs(x) < 1e-6f) 
	{
		return y > 0 ? D3DX_PI / 2 : -D3DX_PI / 2; 
	}
	float a = atanf(y / abs(x));
	if (x < 0) a = D3DX_PI - a;

	return NormAngle(a);
}

//¼ì²éÏÔ¿¨ÄÜÁ¦
HRESULT CheckResourceFormatSupport(IDirect3DDevice9* m_pd3dDevice, D3DFORMAT fmt, D3DRESOURCETYPE resType, DWORD dwUsage)
{
    HRESULT hr = S_OK;
    IDirect3D9* tempD3D = NULL;
    m_pd3dDevice->GetDirect3D(&tempD3D);
    D3DCAPS9 devCaps;
    m_pd3dDevice->GetDeviceCaps(&devCaps);

    D3DDISPLAYMODE displayMode;
    tempD3D->GetAdapterDisplayMode(devCaps.AdapterOrdinal, &displayMode);

    hr = tempD3D->CheckDeviceFormat(devCaps.AdapterOrdinal, devCaps.DeviceType, 
        displayMode.Format, dwUsage, resType, fmt);

    tempD3D->Release(), tempD3D = NULL;

    return hr;
}

void VisualizeTextureLevel(LPDIRECT3DTEXTURE9 tex, int level, int x, int y, int w, int h, float gamma)
{
    LPD3DXEFFECT effect = LoadEffect("utils.fx");
    if (effect == NULL) return;
    CGame2D* g2d = Get2D();
    effect->SetTexture("g_Image", tex);
    effect->SetFloat("g_Level", (float) level);
    effect->SetFloat("g_Gamma", gamma);
    effect->SetTechnique("TextureLevel");
    DX_BEGIN_EFFECT(effect);
    g2d->DrawRect(x, y, w, h, 0xFFFFFFFF);
    DX_END_EFFECT(effect);
}


void MaskTextureLevel(LPDIRECT3DTEXTURE9 tex, int level, int x, int y, int w, int h,
                      float minMask, float maxMask, int maskValue)
{
    LPD3DXEFFECT effect = LoadEffect("utils.fx");
    if (effect == NULL) return;
    CGame2D* g2d = Get2D();
    effect->SetTexture("g_Image", tex);
    effect->SetFloat("g_Level", (float) level);
    effect->SetFloat("g_MinMask", minMask);
    effect->SetFloat("g_MaxMask", maxMask);
    effect->SetInt("g_Mask", maskValue);
    effect->SetTechnique("MaskTexture");
    DX_BEGIN_EFFECT(effect);
    g2d->DrawRect(x, y, w, h, 0xFFFFFFFF);
    DX_END_EFFECT(effect);

}


//--------------------------------------------------------------------------------------
// Desc: Returns a view matrix for rendering to a face of a cubemap.
//--------------------------------------------------------------------------------------
D3DXMATRIX GetCubeMapViewMatrix(float ex, float ey, float ez, DWORD dwFace)
{
    D3DXVECTOR3 vEyePt = D3DXVECTOR3(ex, ey, ez);
    D3DXVECTOR3 vLookDir;
    D3DXVECTOR3 vUpDir;

    switch(dwFace)
    {
    case D3DCUBEMAP_FACE_POSITIVE_X:
        vLookDir = D3DXVECTOR3( 1.0f, 0.0f, 0.0f ) + vEyePt;
        vUpDir   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
        break;
    case D3DCUBEMAP_FACE_NEGATIVE_X:
        vLookDir = D3DXVECTOR3(-1.0f, 0.0f, 0.0f ) + vEyePt;
        vUpDir   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
        break;
    case D3DCUBEMAP_FACE_POSITIVE_Y:
        vLookDir = D3DXVECTOR3( 0.0f, 1.0f, 0.0f ) + vEyePt;
        vUpDir   = D3DXVECTOR3( 0.0f, 0.0f,-1.0f );
        break;
    case D3DCUBEMAP_FACE_NEGATIVE_Y:
        vLookDir = D3DXVECTOR3( 0.0f,-1.0f, 0.0f ) + vEyePt;
        vUpDir   = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
        break;
    case D3DCUBEMAP_FACE_POSITIVE_Z:
        vLookDir = D3DXVECTOR3( 0.0f, 0.0f, 1.0f ) + vEyePt;
        vUpDir   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
        break;
    case D3DCUBEMAP_FACE_NEGATIVE_Z:
        vLookDir = D3DXVECTOR3( 0.0f, 0.0f,-1.0f ) + vEyePt;
        vUpDir   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
        break;
    }

    // Set the view transform for this cubemap surface
    D3DXMATRIXA16 mView;
    D3DXMatrixLookAtLH( &mView, &vEyePt, &vLookDir, &vUpDir );
    return mView;
}

void ScreenToDir(int x, int y, float* dx, float* dy, float* dz)
{
    D3DXMATRIX matView, matProj, matInvView;
    GetDevice()->GetTransform(D3DTS_VIEW, &matView);
    GetDevice()->GetTransform(D3DTS_PROJECTION, &matProj);
    D3DXMatrixInverse(&matInvView, NULL, &matView);

    D3DXVECTOR4 vecDir; 
    vecDir.x = (2 * x / (float)GetScreenWidth()- 1) / matProj._11;
    vecDir.y = (2 * (1 - y / (float)GetScreenHeight()) - 1) / matProj._22;
    vecDir.z = 1;
    vecDir.w = 0;

    D3DXVec4Transform(&vecDir, &vecDir, &matInvView);
    *dx = vecDir.x;
    *dy = vecDir.y;
    *dz = vecDir.z;
}