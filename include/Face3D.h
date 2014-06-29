/************************************************************************/
/* Direct3D GameEngine                                                  */
/* ÃæÆ¬Ä£ÐÍ                   ±àÐ´£º Ò¦´ºêÍ                             */
/************************************************************************/
#pragma once

#include <D3DX9.h>
#include <math.h>
#include <stdio.h>
#include "Object3D.h"

typedef VERTEX_TEX1 VERTEX_FACE;

//Plane
class CFace3D: public CObject3D
{
protected:
	float m_width, m_height;
	void setVertex(VERTEX_FACE& v, FLOAT x, FLOAT y, FLOAT z, DWORD color, FLOAT tu, FLOAT tv);
public:
	float GetWidth() const { return m_width; }
	float GetHeight() const { return m_height; }
	void SetWidth(float w) { m_width = w; }
	void SetHeight(float h) { m_height = h; }

	virtual HRESULT Init(LPDIRECT3DDEVICE9 device);
	virtual void Render();
	void UpdateMatrix();
	void UpdateMatrixBillboard();
	CFace3D(float w, float h);
	virtual ~CFace3D(void);
};

class CAnimationFace3D: public CFace3D
{
protected:
	int m_clipRows, m_clipCols;
	float m_time;
public:
	float Delay;
	int startFrame;
	int endFrame;
	int curFrame;
	void Reset();
	void Animate(float elapsedTime);
	CAnimationFace3D(float w, float h, int r, int c);
	~CAnimationFace3D(void);
};

//Billboard
class CBillboard: public CFace3D
{
public:
	void UpdateMatrix();
	CBillboard(float w, float h);
	~CBillboard(void);
};
