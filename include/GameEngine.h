/************************************************************************/
/* Direct3D GameEngine                                                  */
/* GameEngine                 编写： 姚春晖                             */
/************************************************************************/
#pragma once

#include "GameCommon.h"
#include <Windows.h>
#include "Game2D.h"
#include "Image2DManager.h"
#include "StateStack.h"
#include "MouseManager.h"
#include "KeyManager.h"
#include "EffectManager.h"

#ifdef _USE_SOUND_
#include "FMODSoundPlay.h"
#endif

//#include "ThreadPool.h"

#ifdef _USE_3D_
#include "Camera.h"
#endif

HWND				GetWindow();
LPDIRECT3D9			GetDirect3D();
LPDIRECT3DDEVICE9	GetDevice();
int					GetScreenWidth();
int					GetScreenHeight();
void				QuitGame();
void				PauseGame();
CStateStack *		GetStateStack();
CGame2D *			Get2D();

CMouseManager *		GetMouse();
CKeyManager *		GetKeyboard();

#ifdef _USE_SOUND_
CSoundPlayer*		GetSoundPlayer();
CGameSound *		GetBGM();
void				StopBGM();
void				PlayBGM(CGameSound * music);
#endif

#ifdef _USE_3D_
CCamera *			GetCamera();
#endif

CImage2D * LoadGameImage(const string &filename, D3DCOLOR color = 0);
LPD3DXEFFECT LoadEffect(const string &filename);
LPD3DXFONT	LoadFont(const char * name, UINT size, UINT weight = FW_NORMAL, bool italic = false);

//初始化游戏
bool InitGame(const char * title, int x, int y, int width, int height, bool windowed, bool console);

//主循环
bool RunGame();

//清除所有资源
void Cleanup();
