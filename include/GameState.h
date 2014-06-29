/************************************************************************/
/* Direct3D GameEngine                                                  */
/* 游戏状态                 编写： 姚春晖                               */
/************************************************************************/

#include <stdlib.h>
#include <cstring>
#include "Game2D.h"

#pragma once

/**
 * CGameState类是"游戏状态"的基类."游戏状态"为游戏当前在做什么.
 * 所有状态都在游戏状态栈中,栈顶状态为当前活动状态.
 * 这是一个抽象类，要使用它的派生类
 */

class CGameState
{
friend class StateStack;
protected:
    //系统栈
    CGameState *m_prior;
    //计时器
	bool m_exited;
	//是否需要画前一状态
	bool m_drawPrior;
	//名称
    char m_name[100];
public:
    CGameState(const char *name);
    virtual ~CGameState(void);
	
	void virtual Start(CGameState * prior);
    /**
    * 覆盖这个方法, 加入画图代码
    */
    void virtual Draw2D(CGame2D * g2d);

    void virtual PreRender();

	void virtual Draw3D();

    void virtual PostRender();

    /**
    * 更新计时器
    */
    void virtual Animate(float elapsedTime);

    /**
    * 覆盖这个方法, 加入更新状态代码
    */
	void virtual Update(float elapsedTime);

    bool IsExited() const { return m_exited; }

    const char* GetName() const { return m_name; }

	void virtual DeviceLost() {};
	void virtual DeviceReset() {};
};
