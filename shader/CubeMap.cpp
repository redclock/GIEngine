#include "CubeMap.h"
#include "GameEngine.h"


//D3DCUBEMAP_FACE_POSITIVE_X     = 0,
//D3DCUBEMAP_FACE_NEGATIVE_X     = 1,
//D3DCUBEMAP_FACE_POSITIVE_Y     = 2,
//D3DCUBEMAP_FACE_NEGATIVE_Y     = 3,
//D3DCUBEMAP_FACE_POSITIVE_Z     = 4,
//D3DCUBEMAP_FACE_NEGATIVE_Z     = 5,

float CUBE_LOOK_VECTOR[6][3] =
{
    {1, 0, 0}, {-1, 0, 0}, 
    {0, 1, 0}, {0, -1, 0}, 
    {0, 0, 1}, {0, 0, -1}, 
};

CCubeMap::CCubeMap()
{
    D3DXMatrixPerspectiveFovLH(&m_matProj, D3DX_PI / 2, 1.0f, 1.5f, 500.0f);
    m_cubeMap = NULL;
}

CCubeMap::~CCubeMap()
{
    SAFE_RELEASE(m_cubeMap);
}

bool CCubeMap::CreateTexture(UINT size, D3DFORMAT format, 
                             D3DFORMAT depthFormat, UINT levels, 
                             DWORD usage)
{
    m_size = size;
    m_width = size;
    m_height = size;
    m_format = format;
    m_dformat = depthFormat;
    m_device = GetDevice();
    
    BOOL hasDepth = depthFormat == D3DFMT_UNKNOWN ? FALSE : TRUE;

    if (FAILED(D3DXCreateCubeTexture(GetDevice(), size, levels, 
        D3DUSAGE_RENDERTARGET | usage, 
        format, D3DPOOL_DEFAULT, &m_cubeMap)))
    {
        MessageBox(GetWindow(), "Error creating cube texture.", "Error", 0);
        return false;
    }

    CreateDepthSurface(depthFormat);
    InitViewPort();

    m_level = 0;

    return true;
}

void CCubeMap::SetCenter(float x, float y, float z)
{
    m_center.x = x;
    m_center.y = y;
    m_center.z = z;
}

const D3DXMATRIX CCubeMap::GetViewMatrix(int face) const
{
    return GetCubeMapViewMatrix(m_center.x, m_center.y, m_center.z, face);
}

bool CCubeMap::UpdateSurface(int face, int level)
{
    if (m_surface)
    {
        m_surface->Release();
        m_surface = NULL;
    }
    if (FAILED(m_cubeMap->GetCubeMapSurface((D3DCUBEMAP_FACES) face, level, &m_surface)))
    {
        MessageBox(NULL, "Get Surface Error", "ERROR", 0);
        return false;
    }

    m_matView = GetCubeMapViewMatrix(m_center.x, m_center.y, m_center.z, face);
    m_level = level;
    m_viewport.X = 0;
    m_viewport.Y = 0;
    m_viewport.Width  = m_size / (1 << level);
    m_viewport.Height = m_size / (1 << level);

    return true;
}