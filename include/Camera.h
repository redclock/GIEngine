/************************************************************************/
/* Direct3D GameEngine                                                  */
/* ÉãÏñ»ú                 ±àÐ´£º Ò¦´ºêÍ                                 */
/************************************************************************/

#pragma once

#include <D3DX9.h>
#include <math.h>
#include "MyLib.h"
#include <stdio.h>

class CCamera
{
protected:
	D3DXVECTOR3 vUpVec;
    D3DXVECTOR3 vEyePt;
    D3DXVECTOR3 vLookatPt;
    D3DXMATRIXA16 matView;
    FLOAT m_LiftAngle;
public:
    D3DXVECTOR3 GetUp() const
    {
        return vUpVec;
    };
    D3DXVECTOR3 GetEye() const
    {
        return vEyePt;
    };
    D3DXVECTOR3 GetLookat() const
    {
        return vLookatPt;
    };
    D3DXVECTOR3 GetEyeDirection() const
    {
        return vLookatPt - vEyePt;
    };
	void Turn(FLOAT angle);
	void Lift(FLOAT angle);
    void UpdateMatrix();
	void GoAhead(FLOAT dist);
	 void MoveEye(const D3DXVECTOR3& v);
	 HRESULT SetTransform();

	const D3DXMATRIXA16 * GetViewMatrix() const { return &matView; };

	void SetUp(const D3DXVECTOR3 & v){ vUpVec = v; };
    void SetUp(FLOAT x,FLOAT y, FLOAT z){ vUpVec.x = x; vUpVec.y = y; vUpVec.z = z; };
    void SetEye(const D3DXVECTOR3 & v){ vEyePt = v; };
    void SetEye(FLOAT x,FLOAT y, FLOAT z){ vEyePt.x = x; vEyePt.y = y; vEyePt.z = z; };
    void SetLookat(const D3DXVECTOR3& v){ vLookatPt = v; };
    void SetLookat(FLOAT x, FLOAT y, FLOAT z)
    {
        vLookatPt.x = x; vLookatPt.y = y; vLookatPt.z = z; 
    };

//    char * GetMsg(char * msg) const
//    {
//        D3DXVECTOR3 D = GetEyeDirection();
////        sprintf(msg, "Dir: %f, %f, %f", D.x, D.y, D.z);
//        return msg;
//    }

    CCamera(void);
    ~CCamera(void);
};
