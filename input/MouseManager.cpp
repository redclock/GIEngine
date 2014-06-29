#include "MouseManager.h"
#include "GameEngine.h"

CMouseManager::CMouseManager(void)
{
    m_wheel = 0;	
}

CMouseManager::~CMouseManager(void)
{

}

void CMouseManager::Update()
{
	POINT pt;
    
    LONG style = GetWindowLong(GetWindow(), GWL_STYLE);

    // 窗口已经最小化
    if (style & WS_MINIMIZE)
    {
        LastButtonDown[0] = false;
        LastButtonDown[1] = false;
        return;
    }

    // 得到鼠标位置
	GetCursorPos(&pt);
    // 转化到窗口位置
	ScreenToClient(GetWindow(), &pt);
	
    // 转化到屏幕空间
    RECT rt;
    GetClientRect(GetWindow(), &rt);
    if (rt.left != rt.right)
        pt.x = pt.x * GetScreenWidth() / (rt.right - rt.left);
    if (rt.bottom != rt.top)
        pt.y = pt.y * GetScreenHeight() / (rt.bottom - rt.top);

	if (pt.x < 0) pt.x = 0; else if (pt.x >= GetScreenWidth()) pt.x = GetScreenWidth() - 1;
	if (pt.y < 0) pt.y = 0; else if (pt.y >= GetScreenHeight()) pt.y = GetScreenHeight() - 1;
	
	LastMouseX = MouseX;
	MouseX = pt.x; 
	LastMouseY = MouseY;
	MouseY = pt.y; 

	LastButtonDown[0] = ButtonDown[0];
	LastButtonDown[1] = ButtonDown[1];
	ButtonDown[0] = GetAsyncKeyState(VK_LBUTTON) < 0;
	ButtonDown[1] = GetAsyncKeyState(VK_RBUTTON) < 0;
}