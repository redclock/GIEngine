#pragma once
#include <vector>

#include "GameState.h"

/**
* StatusStack类是盛放游戏状态的栈.
* 所有状态都在游戏状态栈中,栈顶状态为当前活动状态.
* @see AbstractStatus
* @author 姚春晖
*/

class CStateStack
{
private:

    //系统栈
    std::vector< CGameState* > m_stack;

public:
    /**
    * 得到当前状态
    */
    CGameState* GetCurrStatus() const;

   /**
    * 清空栈及状态们的内存
    */
    void Clear();
    
    /**
    * 压入一个状态
    */
    int Push(CGameState *s);
    
    /**
    * 弹出状态, 但不会释放内存
    */
    CGameState* Pop();

    /**
    * 清除栈顶
    */
    void RemoveTop();

	size_t GetCount() const { return m_stack.size(); };

	void DeviceLost();
	void DeviceReset();

    CStateStack();
	~CStateStack();
    
};