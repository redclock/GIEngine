/************************************************************************/
/*  Direct3D GameEngine													*/
/*  键盘输入管理                          编写： 姚春晖                 */
/************************************************************************/

#pragma once
#include <Windows.h>

/**
* KeyManager类是键盘管理器
* 它管理按键信息, 其来源是窗体的键盘事件
* @author 姚春晖
*/
class CKeyManager
{
private:
	BYTE m_bLastKeys[256];
    BYTE m_bKeys[256];

public:

	/**
    * 清空所有键的状态
    */
    void ClearKeyState();

	/**
    * 是否这一次为按下
    */
    bool IsPressed(int key) const
	{
		return (m_bKeys[key] & 0x80) != 0;
	}
    
    /**
    * 是否上一次未抬起而这一次为按下
    */
    bool IsJustPressed(int key) const
	{
		return ((m_bLastKeys[key] & 0x80) == 0) && ((m_bKeys[key] & 0x80) != 0);
	}

	/**
    * 是否上一次按下未而这一次为抬起
    */
    bool IsJustReleased(int key) const
	{
		return ((m_bLastKeys[key] & 0x80) != 0) && ((m_bKeys[key] & 0x80) == 0);
	}

	void Update();

    CKeyManager(void);
    ~CKeyManager(void);
};
