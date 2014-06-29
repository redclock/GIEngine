/************************************************************************/
/*  Direct3D GameEngine													*/
/*   Û±Í ‰»Îπ‹¿Ì                          ±‡–¥£∫ “¶¥∫ÍÕ                 */
/************************************************************************/

#pragma once
#include <Windows.h>

class CMouseManager
{
protected:
	int LastMouseX, LastMouseY;
	int MouseX, MouseY;
	bool ButtonDown[2];
	bool LastButtonDown[2];
    int m_wheel;

public:
	int GetX() const { return MouseX; }
	int GetY() const { return MouseY; }
	
	bool IsPressed(int button) const { return ButtonDown[button]; }
	bool IsJustPressed(int button) const { return !LastButtonDown[button] && ButtonDown[button]; }
	bool IsJustReleased(int button) const { return LastButtonDown[button] && !ButtonDown[button]; }

	bool MouseMoved() const { return MouseX != LastMouseX || MouseY != LastMouseY; };
	
    void SetWheel(int wheel) { m_wheel = wheel; };
    int GetWheel() const { return m_wheel; };
	void Update();
	CMouseManager(void);
	~CMouseManager(void);
};
