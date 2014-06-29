/************************************************************************/
/* Direct3D GameEngine                                                  */
/* 设置雾化效果       编写： 姚春晖                                     */
/************************************************************************/

#pragma once

#include <d3dx9.h>

class CFog
{
public:
	//雾类型 
	//  D3DFOG_NONE = 0,
	//	D3DFOG_EXP = 1,
	//	D3DFOG_EXP2 = 2,
	//	D3DFOG_LINEAR = 3,
	static void SetFogMode(D3DFOGMODE mode);
	
	//线性雾起点
	static void SetStart(float v);
	//线性雾终点
	static void SetEnd(float v);
	//指数雾密度
	static void SetDensity(float v);
	//颜色
	static void SetColor(DWORD color);
	static void Enable(BOOL v);

};