#include ".\joyinput.h"
CJoyInput* g_input = NULL;
CJoyInput::CJoyInput(HWND wnd):CInputDevice(wnd)
{
    m_pJoystick = NULL;
    g_input = this;
}

CJoyInput::~CJoyInput(void)
{
    FreeDirectInput();
}

BOOL CALLBACK EnumObjectsCallback( const DIDEVICEOBJECTINSTANCE* pdidoi,
                                  VOID* pContext )
{
    HWND hDlg = (HWND)pContext;

    static int nSliderCount = 0;  // Number of returned slider controls
    static int nPOVCount = 0;     // Number of returned POV controls

    // For axes that are returned, set the DIPROP_RANGE property for the
    // enumerated axis in order to scale min/max values.
    if( pdidoi->dwType & DIDFT_AXIS )
    {
        DIPROPRANGE diprg; 
        diprg.diph.dwSize       = sizeof(DIPROPRANGE); 
        diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
        diprg.diph.dwHow        = DIPH_BYID; 
        diprg.diph.dwObj        = pdidoi->dwType; // Specify the enumerated axis
        diprg.lMin              = -1000; 
        diprg.lMax              = +1000; 

        // Set the range for the axis
        if( FAILED( g_input->SetProperty( DIPROP_RANGE, &diprg.diph ) ) ) 
            return DIENUM_STOP;

    }
    return DIENUM_CONTINUE;
}

//-----------------------------------------------------------------------------
// Name: EnumJoysticksCallback()
// Desc: Called once for each enumerated joystick. If we find one, create a
//       device interface on it so we can play with it.
//-----------------------------------------------------------------------------
BOOL CALLBACK EnumJoysticksCallback( const DIDEVICEINSTANCE* pdidInstance,
                                    VOID* pContext )
{
    HRESULT hr;
    LPDIRECTINPUTDEVICE8 pJoystick;     

    // Obtain an interface to the enumerated joystick.
    hr = g_input->GetDirectInput()->CreateDevice( pdidInstance->guidInstance, &pJoystick, NULL );
    g_input->SetDevice(pJoystick);
    // If it failed, then we can't use this joystick. (Maybe the user unplugged
    // it while we were in the middle of enumerating it.)
    if( FAILED(hr) ) 
        return DIENUM_CONTINUE;

    // Stop enumeration. Note: we're just taking the first joystick we get. You
    // could store all the enumerated joysticks and let the user pick.
    return DIENUM_STOP;
}

HRESULT CJoyInput::InitDevice()
{
    HRESULT hr;

    // Register with the DirectInput subsystem and get a pointer
    // to a IDirectInput interface we can use.
    // Create a DInput object
    if (m_pDI == NULL)
        if( FAILED( hr = InitDirectInput() ) )
            return hr;
    // Look for a simple joystick we can use for this sample program.
    if( FAILED( hr = m_pDI->EnumDevices( DI8DEVCLASS_GAMECTRL, 
        EnumJoysticksCallback,
        NULL, DIEDFL_ATTACHEDONLY ) ) )
        return hr;
    
    // Make sure we got a joystick
    if( NULL == m_pJoystick )
    {
    //    MessageBox( NULL, TEXT("Joystick not found. The sample will now exit."),  
    //        TEXT("DirectInput Sample"), 
    //        MB_ICONERROR | MB_OK );
    //    EndDialog( hDlg, 0 );
        return S_OK;
    }

    // Set the data format to "simple joystick" - a predefined data format 
    //
    // A data format specifies which controls on a device we are interested in,
    // and how they should be reported. This tells DInput that we will be
    // passing a DIJOYSTATE2 structure to IDirectInputDevice::GetDeviceState().
    if( FAILED( hr = m_pJoystick->SetDataFormat( &c_dfDIJoystick2 ) ) )
        return hr;

    // Set the cooperative level to let DInput know how this device should
    // interact with the system and with other DInput applications.
    if( FAILED( hr = m_pJoystick->SetCooperativeLevel( m_wnd, DISCL_EXCLUSIVE | 
        DISCL_FOREGROUND ) ) )
        return hr;
    if( FAILED( hr = m_pJoystick->EnumObjects( EnumObjectsCallback, 
        (VOID*)m_wnd, DIDFT_ALL ) ) )
        return hr;

    return S_OK;
}

HRESULT CJoyInput::Update()
{
    HRESULT     hr;
    memset(&js, 0, sizeof(js));
    if( NULL == m_pJoystick ) 
        return S_OK;

    // Poll the device to read the current state
    hr = m_pJoystick->Poll(); 
    if( FAILED(hr) )  
    {
        // DInput is telling us that the input stream has been
        // interrupted. We aren't tracking any state between polls, so
        // we don't have any special reset that needs to be done. We
        // just re-acquire and try again.
        hr = m_pJoystick->Acquire();
        while( hr == DIERR_INPUTLOST ) 
            hr = m_pJoystick->Acquire();

        // hr may be DIERR_OTHERAPPHASPRIO or other errors.  This
        // may occur when the app is minimized or in the process of 
        // switching, so just try again later 
        return S_OK; 
    }

    // Get the input's device state
    if( FAILED( hr = m_pJoystick->GetDeviceState( sizeof(DIJOYSTATE2), &js ) ) )
        return hr; // The device should have been acquired during the Poll()
 
    return S_OK;
}
VOID CJoyInput::FreeDevice()
{
    // Unacquire the device one last time just in case 
    // the app tried to exit while the device is still acquired.
    if( m_pJoystick ) 
        m_pJoystick->Unacquire();
    // Release any DirectInput objects.
    SAFE_RELEASE( m_pJoystick );
}
bool CJoyInput::IsDown(int Key)
{
    switch(Key) {
    case DIK_W:
        return js.lY <= -100; 
    	break;
    case DIK_S:
        return js.lY >= +100;
        break;
    case DIK_A:
        return (js.lX) <= -100;
        break;
    case DIK_D:
        return (js.lX) >= 100;
        break;
    case DIK_UP:
        return (js.rgbButtons[0] & 0x80) != 0;
        break;
    case DIK_DOWN:
        return (js.rgbButtons[2] & 0x80) != 0;
        break;
    case DIK_RETURN:
        return (js.rgbButtons[9] & 0x80) != 0;
        break;

    }
    return false;
}
