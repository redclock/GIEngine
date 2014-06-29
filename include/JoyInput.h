#pragma once
#include "inputdevice.h"

class CJoyInput :
    public CInputDevice
{
    LPDIRECTINPUTDEVICE8 m_pJoystick;     
    DIJOYSTATE2 js;           // DInput joystick state 
    
public:
    bool Found(){ return m_pJoystick != NULL; };
    void SetDevice(LPDIRECTINPUTDEVICE8 joy)
    {
        m_pJoystick = joy;
    }
    
    HRESULT InitDevice();
    VOID FreeDevice();
    HRESULT Update();
    bool IsDown(int Key);
    HRESULT SetProperty( REFGUID rguidProp, LPCDIPROPHEADER pdiph )
    {
        return m_pJoystick->SetProperty(rguidProp, pdiph);
    }

    LPDIRECTINPUT8 GetDirectInput()
    {
        return m_pDI;
    }
    CJoyInput(HWND wnd);
    ~CJoyInput(void);
};
