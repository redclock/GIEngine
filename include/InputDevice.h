#pragma once
#define STRICT
#define DIRECTINPUT_VERSION 0x0800
#include <tchar.h>
#include <windows.h>
#include <commctrl.h>
#include <basetsd.h>
#include <dinput.h>
#include "GameCommon.h"

//#include "DXUtil.h"
class CInputDevice
{
protected: 
    static LPDIRECTINPUT8       m_pDI; // The DirectInput object 
    HWND m_wnd;

public:
    HRESULT InitDirectInput();
    VOID FreeDirectInput()
    {
        FreeDevice();
        SAFE_RELEASE( m_pDI );
    }
    virtual HRESULT InitDevice() = 0;
    virtual VOID FreeDevice() = 0;
    virtual HRESULT Update() = 0;
    virtual bool IsDown(int Key) = 0;
    CInputDevice(HWND wnd);
    virtual ~CInputDevice(void);
};
