#pragma once
#include <vector>

#include "GameState.h"

/**
* StatusStack����ʢ����Ϸ״̬��ջ.
* ����״̬������Ϸ״̬ջ��,ջ��״̬Ϊ��ǰ�״̬.
* @see AbstractStatus
* @author Ҧ����
*/

class CStateStack
{
private:

    //ϵͳջ
    std::vector< CGameState* > m_stack;

public:
    /**
    * �õ���ǰ״̬
    */
    CGameState* GetCurrStatus() const;

   /**
    * ���ջ��״̬�ǵ��ڴ�
    */
    void Clear();
    
    /**
    * ѹ��һ��״̬
    */
    int Push(CGameState *s);
    
    /**
    * ����״̬, �������ͷ��ڴ�
    */
    CGameState* Pop();

    /**
    * ���ջ��
    */
    void RemoveTop();

	size_t GetCount() const { return m_stack.size(); };

	void DeviceLost();
	void DeviceReset();

    CStateStack();
	~CStateStack();
    
};