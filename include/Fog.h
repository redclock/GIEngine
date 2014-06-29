/************************************************************************/
/* Direct3D GameEngine                                                  */
/* ������Ч��       ��д�� Ҧ����                                     */
/************************************************************************/

#pragma once

#include <d3dx9.h>

class CFog
{
public:
	//������ 
	//  D3DFOG_NONE = 0,
	//	D3DFOG_EXP = 1,
	//	D3DFOG_EXP2 = 2,
	//	D3DFOG_LINEAR = 3,
	static void SetFogMode(D3DFOGMODE mode);
	
	//���������
	static void SetStart(float v);
	//�������յ�
	static void SetEnd(float v);
	//ָ�����ܶ�
	static void SetDensity(float v);
	//��ɫ
	static void SetColor(DWORD color);
	static void Enable(BOOL v);

};