/************************************************************************/
/*  Direct3D GameEngine													*/
/*  �����������                          ��д�� Ҧ����                 */
/************************************************************************/

#pragma once
#include <Windows.h>

/**
* KeyManager���Ǽ��̹�����
* ����������Ϣ, ����Դ�Ǵ���ļ����¼�
* @author Ҧ����
*/
class CKeyManager
{
private:
	BYTE m_bLastKeys[256];
    BYTE m_bKeys[256];

public:

	/**
    * ������м���״̬
    */
    void ClearKeyState();

	/**
    * �Ƿ���һ��Ϊ����
    */
    bool IsPressed(int key) const
	{
		return (m_bKeys[key] & 0x80) != 0;
	}
    
    /**
    * �Ƿ���һ��δ̧�����һ��Ϊ����
    */
    bool IsJustPressed(int key) const
	{
		return ((m_bLastKeys[key] & 0x80) == 0) && ((m_bKeys[key] & 0x80) != 0);
	}

	/**
    * �Ƿ���һ�ΰ���δ����һ��Ϊ̧��
    */
    bool IsJustReleased(int key) const
	{
		return ((m_bLastKeys[key] & 0x80) != 0) && ((m_bKeys[key] & 0x80) == 0);
	}

	void Update();

    CKeyManager(void);
    ~CKeyManager(void);
};
