#include "Camera.h"
#include "GameEngine.h"

CCamera::CCamera(void)
{
    m_LiftAngle = 0.0f;
    SetUp(0, 1, 0);
    SetLookat(0, 0, 0);
    SetEye(1, 0, 1);
    UpdateMatrix();
}

CCamera::~CCamera(void)
{
}

void CCamera::UpdateMatrix()
{        
	D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
}

HRESULT CCamera::SetTransform()
{
	return GetDevice()->SetTransform( D3DTS_VIEW, &matView );
}

void CCamera::Turn(FLOAT angle)
{
	D3DXVECTOR3 D = GetEyeDirection();
	D3DXVECTOR3 t;
	t.y = D.y;
	t.x = D.x*cosf(angle)-D.z*sinf(angle);
	t.z = D.z*cosf(angle)+D.x*sinf(angle);
	vLookatPt = vEyePt + t;
}

void CCamera::Lift(FLOAT angle)
{
	m_LiftAngle += angle;
	D3DXVECTOR3 D = GetEyeDirection();
	FLOAT len = D3DXVec3LengthSq(&D);
	D.y = sqrtf(len) * sinf(m_LiftAngle);
	if (D.z == 0)
	{
		int f = D.x>= 0? 1:-1;
		D.x = f*sqrtf((len - D.y*D.y));
	}else{
		int f = D.z>= 0? 1:-1;
		FLOAT k = D.x/D.z;
		D.z = f*sqrtf((len - D.y*D.y)/(k*k+1));
		D.x = D.z * k;
	}
	vLookatPt = vEyePt + D;
}

void CCamera::GoAhead(FLOAT dist)
{
	D3DXVECTOR3 D = GetEyeDirection();
	D3DXVec3Normalize(&D, &D);
	D *= dist;
	vLookatPt += D;
	vEyePt += D;
}

void CCamera::MoveEye(const D3DXVECTOR3& v)
{
	D3DXVECTOR3 D = GetEyeDirection();
	vEyePt = v;
	vLookatPt = vEyePt +D;
}