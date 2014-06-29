#define   _WIN32_WINNT           0x0400 

#include "GameEngine.h"
#include "MyLib.h"

#include "FontManager.h"
#include "Log.h"

LPDIRECT3D9             pDirect3D       = NULL; // DirectD����
LPDIRECT3DDEVICE9       pDevice			= NULL; // DirectD�豸����
HWND					hWnd			= NULL; // ������
CGame2D *				pGame2D			= NULL; // 2D����
CImage2DManager *		pImageManager	= NULL; 
CFontManager *			pFontManager	= NULL;
CStateStack *			pStateStack		= NULL;
CMouseManager*			pMouse			= NULL;
CKeyManager*			pKeyboard		= NULL;
CEffectManager*         pEffectManger   = NULL;

#ifdef _USE_SOUND_
CSoundPlayer*			pSoundPlayer	= NULL;
int BGM_Channel = -1;
CGameSound * BGMSound;
#endif

#ifdef _USE_3D_
CCamera					Camera;
#endif

//�ڲ�״̬
bool bQuit;
bool bPause;
bool bDeviceLost;
//�Ƿ񴰿ڻ�
bool bWindowed;
//�Ƿ���ʾ����̨
bool bConsole;

int ScreenWidth;
int ScreenHeight;

//FPS����
int FPS;

bool bShowFPS = false;

LPD3DXFONT FPSFont = NULL;

int GetScreenWidth()
{
	return ScreenWidth;
}

int GetScreenHeight()
{
	return ScreenHeight;
}

void QuitGame()
{
	bQuit = true;
}

void PauseGame()
{
	bPause = true;
}

HWND GetWindow()
{
	return hWnd;
}

LPDIRECT3D9 GetDirect3D()
{
	return pDirect3D;
}

LPDIRECT3DDEVICE9 GetDevice()
{
	return pDevice;
}

CStateStack * GetStateStack()
{
	return pStateStack;
}

CGame2D* Get2D()
{
	return pGame2D; 
}

CMouseManager * GetMouse()
{
	return pMouse;
}

CKeyManager * GetKeyboard()
{
	return pKeyboard;
}

#ifdef _USE_SOUND_
CSoundPlayer * GetSoundPlayer()
{
	return pSoundPlayer;
}

CGameSound * GetBGM()
{
	return BGMSound;
}

void StopBGM()
{
	if (!pSoundPlayer) return;
	if (BGM_Channel >= 0)
	{
		pSoundPlayer->StopSound(BGM_Channel);
		BGM_Channel = -1;
	}
	if (BGMSound)
	{
		//BGMSound->release();
		BGMSound = NULL;
	}
}

void PlayBGM(CGameSound * music)
{
	if (!pSoundPlayer) return;
	StopBGM();
	if (music)
	{
		BGM_Channel = pSoundPlayer->Play(music);
		BGMSound = music;
	}
}
#endif 

#ifdef _USE_3D_
CCamera * GetCamera()
{
	return &Camera;
}
#endif

//���ô��ڵĿͻ�����С
void SetClientRect(HWND hWnd, RECT rt)
{
	AdjustWindowRectEx(&rt, GetWindowLong(hWnd, GWL_STYLE), FALSE,
		GetWindowLong(hWnd, GWL_EXSTYLE));

	MoveWindow(hWnd, rt.left, rt.top, rt.right - rt.left, rt.bottom - rt.top, FALSE);

}

CImage2D * LoadGameImage(const string &filename, D3DCOLOR color)
{
	return pImageManager->AddImage(pDevice, filename, color);
}

LPD3DXFONT	LoadFont(const char * name, UINT size, UINT weight, bool italic)
{
	return pFontManager->AddFont(name, size, weight, italic);
}


LPD3DXEFFECT LoadEffect(const string &filename)
{
    return pEffectManger->LoadEffect(pDevice, filename);
}

void OnDeviceLost()
{
	if (pGame2D) pGame2D->OnLostDevice();
	if (pStateStack) pStateStack->DeviceLost();
    if (pFontManager) pFontManager->OnLostDevice();
    if (pEffectManger) pEffectManger->OnLostDevice();
}

void OnDeviceReset()
{
	if (pGame2D) pGame2D->OnResetDevice();
	if (pStateStack) pStateStack->DeviceReset();
	if (pFontManager) pFontManager->OnResetDevice();
    if (pEffectManger) pEffectManger->OnResetDevice();

	//����Ĭ����Ⱦ״̬
	BeginAlpha();
	SetEffect(fxBlend, true);

	pDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	pDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	pDevice->SetRenderState( D3DRS_ZENABLE, TRUE );

	//Projection Transform
	float fAspectRatio = ScreenWidth / (FLOAT)ScreenHeight;
	D3DXMATRIXA16 matProj;
	D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, fAspectRatio, 10.0f, 100000.0f );
	pDevice->SetTransform( D3DTS_PROJECTION, &matProj );
}


HRESULT CheckDeviceCaps(const D3DCAPS9 & caps)
{
	//���VS��PS�汾
	GetLog() << CLog::TYPE_ENGINE << "Vertex shader version: " << 
		(int)(D3DSHADER_VERSION_MAJOR(caps.VertexShaderVersion)) << '.' << 
		(int)(D3DSHADER_VERSION_MINOR(caps.VertexShaderVersion));
	GetLog() << CLog::TYPE_ENGINE << "Pixel shader version: " << 
		(int)(D3DSHADER_VERSION_MAJOR(caps.PixelShaderVersion)) << '.' << 
		(int)(D3DSHADER_VERSION_MINOR(caps.PixelShaderVersion));
	//if( caps.VertexShaderVersion < D3DVS_VERSION(1, 1) )
	//{
	//	GetLog() << CLog::TYPE_ERROR << "Vertex shader 1.1 required";
	//	return E_FAIL;
	//}	
	//if( caps.PixelShaderVersion < D3DPS_VERSION(1, 4) )
	//{
	//	GetLog() << CLog::TYPE_ERROR << "Pixel shader 1.4 required";
	//	return E_FAIL;
	//}	
	return S_OK;
}

//��ʼ��Direct3D
HRESULT InitD3D( HWND hWnd, bool windowed )
{
	bWindowed = windowed;
	// ���� D3D ����.
	if( NULL == ( pDirect3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
		return E_FAIL;

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory( &d3dpp, sizeof(d3dpp) );
	d3dpp.Windowed = bWindowed;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	d3dpp.BackBufferCount = 1;
	d3dpp.BackBufferWidth = ScreenWidth;
	d3dpp.BackBufferHeight = ScreenHeight;
	d3dpp.FullScreen_RefreshRateInHz = 0;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
    d3dpp.Flags = 0;
	// ���� D3DDevice
	if( FAILED( pDirect3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dpp, &pDevice ) ) )
	{
		return E_FAIL;
	}
	
	//���Caps
	D3DCAPS9 caps;
	pDevice->GetDeviceCaps(&caps);

	if (FAILED(CheckDeviceCaps(caps)))
	{
		SAFE_RELEASE(pDevice);
		return E_FAIL;
	}

	return S_OK;
}

//�ؽ��豸
HRESULT ResetDevice()
{
	HRESULT hr;
	// ����cooperative level
	if( FAILED( hr = pDevice->TestCooperativeLevel() ) )
	{
		if( D3DERR_DEVICELOST == hr )
		{
			// ��ǰ�������ؽ��豸.  
			Sleep( 50 );
			return hr;
		}
	}

	//�����ؽ�
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory( &d3dpp, sizeof(d3dpp) );
	d3dpp.Windowed = bWindowed;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	d3dpp.BackBufferCount = 1;
	d3dpp.BackBufferWidth = ScreenWidth;
	d3dpp.BackBufferHeight = ScreenHeight;
	d3dpp.FullScreen_RefreshRateInHz = 0;
	hr = pDevice->Reset( &d3dpp );
	if( FAILED(hr) )  //�ؽ�ʧ��
	{
		if( hr == D3DERR_DEVICELOST )
			return D3DERR_DEVICELOST;
        else if ( hr == D3DERR_DRIVERINTERNALERROR )
            return D3DERR_DRIVERINTERNALERROR;
        else if ( hr == D3DERR_OUTOFVIDEOMEMORY )
            return D3DERR_OUTOFVIDEOMEMORY;
		else
			return hr;
	}

	//�ɹ����ؽ���
	bDeviceLost = false;
	OnDeviceReset();
	
	//����Ǵ��ڻ�, �����������ô��ڴ�С
	if (bWindowed)
	{
		RECT rt = {100, 100, 100 + ScreenWidth, 100 + ScreenHeight};
		SetClientRect(hWnd, rt);
	}
	return S_OK;
}

//���������Դ
void Cleanup()
{
	OnDeviceLost();

	GetLog() << CLog::TYPE_ENGINE << "���������Դ";

	SAFE_DELETE(pStateStack);
	SAFE_DELETE(pGame2D);
	SAFE_DELETE(pImageManager);
	SAFE_DELETE(pFontManager);
	SAFE_DELETE(pMouse);
	SAFE_DELETE(pKeyboard);
    SAFE_DELETE(pEffectManger);

#ifdef _USE_SOUND_
	SAFE_DELETE(pSoundPlayer);
#endif

	// �ͷ�D3D�豸, �����ǿ�����
	if( pDevice )
	{
		if( pDevice->Release() > 0 )  
		{
			GetLog() << CLog::TYPE_WARNING << "Direct3D���ڷǿ�����";
		}
	}

	//SAFE_RELEASE(pDevice);

	SAFE_RELEASE(pDirect3D);

	if (bConsole)
	{
		GetLog() << CLog::TYPE_ENGINE << "�������̨";
		FreeConsole();
	}
}

//
void Render()
{
	pDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
		0xFFAA99EE, 1.0f, 0 );
	HRESULT hr;

    CGameState* s = pStateStack->GetCurrStatus();

    if (s)
    {
        s->PreRender();

        // Begin the scene
	    if( SUCCEEDED(hr = pDevice->BeginScene() ) )
	    {
			//DRAW 3D
#ifdef _USE_3D_
			Camera.SetTransform();
			s->Draw3D();
#endif
			////draw 2d
			pGame2D->BeginDraw();
			s->Draw2D(pGame2D);
			if (FPSFont == NULL)
				FPSFont = LoadFont("Arial", 20);
			else
			{
                if (bShowFPS)
                {
				    SetEffect(fxBlend);
				    char  txt[100];
				    wsprintf(txt, "FPS = %d.%d", FPS / 10, FPS % 10);
				    if (bDeviceLost)
					    strcat(txt, " (lost)");
                    
				    pGame2D->DrawStringShadow(FPSFont, txt, 0,  20, 0xFFAAFFAA);
                }
			}
			pGame2D->EndDraw();
    		// End the scene
	    	pDevice->EndScene();

            s->PostRender();
        }
        else
        {
            return;
        }
	}
    if(!bQuit)
    {
	    hr = pDevice->Present( NULL, NULL, NULL, NULL );

	    //����Ƿ��Ѿ���ʧ�豸
	    if (FAILED(hr))
	    {
		    if (hr == D3DERR_DEVICELOST)
		    {
			    bDeviceLost = true;
			    OnDeviceLost();
		    }	
	    }
    }
}


//elaspedTime�Ǻ�����
void Update(int elapsedTime)
{
	pMouse->Update();
	pKeyboard->Update();

	CGameState* s = pStateStack->GetCurrStatus();
	if (s)
	{
		s->Animate(elapsedTime * 0.001f);
		s->Update(elapsedTime * 0.001f);
	}
    pMouse->SetWheel(0);
    if (pKeyboard->IsJustPressed('F') && pKeyboard->IsPressed(VK_LCONTROL) && pKeyboard->IsPressed(VK_LSHIFT))
        bShowFPS = !bShowFPS;
}

LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch( msg )
	{
    case WM_MOUSEWHEEL:
        pMouse->SetWheel(GET_WHEEL_DELTA_WPARAM(wParam));
        return 0;
	case WM_DESTROY:
		PostQuitMessage( 0 );
        bQuit = true;
		return 0;
	}

	return DefWindowProc( hWnd, msg, wParam, lParam );
}

//��������
void CreateGameWindow( const char * title, int x, int y, int width, int height )
{
	GetLog() << CLog::TYPE_ENGINE << "��������: Pos(" << x << ',' << y << "); Size(" << width << ',' << height << ")";
	
	// ע�ᴰ����
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
		GetModuleHandle(NULL), NULL, LoadCursor(NULL, IDC_ARROW), NULL, NULL,
		"MYD3D APP", NULL };
	RegisterClassEx( &wc );

	//��������
	hWnd = CreateWindow(wc.lpszClassName, title,
		WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX  | WS_SIZEBOX | WS_POPUP, 
		x, y, width, height,
		GetDesktopWindow(), NULL, wc.hInstance, NULL );
	ScreenWidth = width;
	ScreenHeight = height;

	SetClientRect(hWnd, MakeBounds(100, 100, ScreenWidth, ScreenHeight));

	RECT rt;
	GetWindowRect(hWnd, &rt);
	int W = rt.right - rt.left;
	int H = rt.bottom - rt.top;

	int ScreenW = GetSystemMetrics(SM_CXSCREEN);
	int ScreenH = GetSystemMetrics(SM_CYSCREEN);
	//MoveWindow(hWnd, (ScreenW - W) / 2, (ScreenH - H) / 2, W, H, FALSE);

}

//��ʼ����Ϸ
bool InitGame(const char * title, int x, int y, int width, int height, bool windowed, bool console)
{
	//��ʼHTML�ļ�¼
	BeginHTMLLog("log.html");

	GetLog() << CLog::TYPE_ENGINE << "��ʼ����Ϸ����";
	
	srand(timeGetTime());
	bQuit = false;
	bPause = false;
	bConsole = console;
	CreateGameWindow(title, x, y, width, height);
	
	//��������̨
	if (bConsole)
	{
		AllocConsole();
		freopen("CONOUT$","w+t",stdout); 
		freopen("CONIN$","r+t",stdin); 
		GetLog() << CLog::TYPE_ENGINE << "��������̨";
	}

	//����D3D�豸
	bool succ = SUCCEEDED(InitD3D(hWnd, windowed));
	
	if (succ)
	{
		//�������ֶ���
		GetLog() << CLog::TYPE_ENGINE << "Direct3D�豸��ʼ���ɹ�";
		
		pGame2D = new CGame2D();

		pImageManager = new CImage2DManager();

		pStateStack = new CStateStack();

		pFontManager = new CFontManager();

        pEffectManger = new CEffectManager();

		pGame2D->Init(pDevice);

		pMouse = new CMouseManager();
		pKeyboard = new CKeyManager();

#ifdef _USE_SOUND_
		pSoundPlayer = new CSoundPlayer(10);
		pSoundPlayer->InitSystem();
#endif

		OnDeviceReset();

		// Show the window
		ShowWindow( hWnd, SW_SHOWDEFAULT );
		UpdateWindow( hWnd );
	}
	else
	{
		GetLog() << CLog::TYPE_ERROR << "Direct3D�豸��ʼ��ʧ��";
	}
	return succ;
}


//��ѭ��
bool RunGame()
{
	
	//��һ֡ʱ��
	int oldtime = GetTickCount();
	//��һ�μ���FPSʱ��
	int oldFPSTime = GetTickCount();
	//��ǰʱ��
	int time = oldtime;
	//��Ⱦ֡��
    int frameCount[10] = {0};
    int frameP = 0;
    FPS = 0;

	GetLog() << CLog::TYPE_ENGINE << "��ʼ������Ϸ";

	// Enter the message loop
	MSG msg;
	ZeroMemory( &msg, sizeof(msg) );

	while( msg.message != WM_QUIT && !bQuit)
	{
		//��ȥ��ʱ��
		int elapsed = -oldtime + GetTickCount();

		if (elapsed > 100) elapsed = 100; 

		if (GetFocus() != hWnd) Sleep(100);

		oldtime = GetTickCount();

		time = oldtime;
		//�������1��,����FPS
		if (time - oldFPSTime >= 1000)
		{
			oldFPSTime = time;
            FPS += frameCount[frameP];
			frameP ++;
            if (frameP == 10)
                frameP = 0;
            FPS -= frameCount[frameP];
            frameCount[frameP] = 0;
		}

#ifdef _USE_SOUND_
		if (pSoundPlayer) pSoundPlayer->SoundLoop();
#endif

		if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		if (bDeviceLost)
		{
			ResetDevice();
		}
		else if (!bPause && !bQuit)
		{
			Render();
			Update(elapsed);
		}
		frameCount[frameP]++;
	}
	GetLog() << CLog::TYPE_ENGINE << "����������Ϸ";
	Cleanup();
	
	return 0;
}


