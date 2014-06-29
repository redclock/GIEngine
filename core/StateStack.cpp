#include "StateStack.h"

/* 
 *StatusStack类的实现
 */

CStateStack::CStateStack()
{
    
}


CStateStack::~CStateStack(void)
{
    Clear();
}

//如果空则返回NULL
CGameState* CStateStack::GetCurrStatus() const
{
    if (m_stack.empty())
        return NULL;
    else
        return m_stack.back();
}


void CStateStack::Clear()
{
    CGameState* s;
    while(! m_stack.empty())
    {
        s = Pop();
        delete s;
    }
}

int CStateStack::Push(CGameState *s)
{
    CGameState *p = GetCurrStatus();
    m_stack.push_back(s);
    // 返回序号
	s->Start(p);
    return (int) m_stack.size() - 1;
} 


CGameState* CStateStack::Pop()
{
    
    CGameState* s = GetCurrStatus();
    m_stack.pop_back();
    return s;
}

void CStateStack::RemoveTop()
{
    CGameState *s = Pop();
    if (s) delete(s);

}

void CStateStack::DeviceLost()
{
	for (size_t i = 0; i< m_stack.size(); i++)
	{
		m_stack[i]->DeviceLost();
	}
}

void CStateStack::DeviceReset()
{
	for (size_t i = 0; i< m_stack.size(); i++)
	{
		m_stack[i]->DeviceReset();
	}
}