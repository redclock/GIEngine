#include "InputDevice.h"

LPDIRECTINPUT8 CInputDevice::m_pDI = NULL;
CInputDevice::CInputDevice(HWND wnd)
{
    m_wnd = wnd;
}

CInputDevice::~CInputDevice(void)
{
}

HRESULT CInputDevice::InitDirectInput()
{
    if (m_pDI) return S_OK;
    HRESULT hr;
    
    // Create a DInput object
    if( FAILED( hr = DirectInput8Create( GetModuleHandle(NULL), DIRECTINPUT_VERSION, 
        IID_IDirectInput8, (VOID**)&m_pDI, NULL ) ) )
        return hr;
    return S_OK;
}
