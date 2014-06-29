
#pragma once
#include "inputdevice.h"

class CKeyInput :
    public CInputDevice
{
    LPDIRECTINPUTDEVICE8 m_pKeyboard; // The input device 
    BYTE    diks[256];   // DirectInput keyboard state buffer 

public:
    HRESULT InitDevice();
    VOID FreeDevice();
    HRESULT Update();
    bool IsDown(int Key);

    CKeyInput(HWND wnd);
    ~CKeyInput(void);
};
