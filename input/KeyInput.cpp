#include ".\keyinput.h"

CKeyInput::CKeyInput(HWND wnd):CInputDevice(wnd)
{
    m_pKeyboard =NULL;
}

CKeyInput::~CKeyInput(void)
{
    FreeDirectInput();
}
HRESULT CKeyInput::InitDevice()
{
    HRESULT hr;
    if (m_pDI == NULL)
        if( FAILED( hr = InitDirectInput() ) )
            return hr;
    if( FAILED( hr = m_pDI->CreateDevice( GUID_SysKeyboard, &m_pKeyboard, NULL ) ) )
        return hr;
    DWORD dwCoopFlags = DISCL_NONEXCLUSIVE | DISCL_FOREGROUND | DISCL_NOWINKEY;

    // Set the data format to "keyboard format" - a predefined data format 
    //
    // A data format specifies which controls on a device we
    // are interested in, and how they should be reported.
    //
    // This tells DirectInput that we will be passing an array
    // of 256 bytes to IDirectInputDevice::GetDeviceState.
    if( FAILED( hr = m_pKeyboard->SetDataFormat( &c_dfDIKeyboard ) ) )
        return hr;

    // Set the cooperativity level to let DirectInput know how
    // this device should interact with the system and with other
    // DirectInput applications.
    hr = m_pKeyboard->SetCooperativeLevel( m_wnd, dwCoopFlags );

    if( FAILED(hr) )
        return hr;



    // Acquire the newly created device
    m_pKeyboard->Acquire();
    return S_OK;
}

VOID CKeyInput::FreeDevice()
{
    // Unacquire the device one last time just in case 
    // the app tried to exit while the device is still acquired.
    if( m_pKeyboard ) 
        m_pKeyboard->Unacquire();
    // Release any DirectInput objects.
    SAFE_RELEASE( m_pKeyboard );
}

HRESULT CKeyInput::Update()
{
    HRESULT hr;

    if( NULL == m_pKeyboard ) 
        return S_OK;

    // Get the input's device state, and put the state in dims
    ZeroMemory( diks, sizeof(diks) );
    hr = m_pKeyboard->GetDeviceState( sizeof(diks), diks );
    if( FAILED(hr) ) 
    {
        // DirectInput may be telling us that the input stream has been
        // interrupted.  We aren't tracking any state between polls, so
        // we don't have any special reset that needs to be done.
        // We just re-acquire and try again.

        // If input is lost then acquire and keep trying 
        hr = m_pKeyboard->Acquire();
        while( hr == DIERR_INPUTLOST ) 
            hr = m_pKeyboard->Acquire();

        return S_OK; 
    }

    return S_OK;
}

bool CKeyInput::IsDown(int Key)
{
    if (Key >= 0 && Key < 256)
        return ( (diks[Key] & 0x80) != 0 );
    else
        return false;
}

