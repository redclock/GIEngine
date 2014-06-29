/************************************************************************/
/* Direct3D GameEngine                                                  */
/* ��Ϸ״̬                 ��д�� Ҧ����                               */
/************************************************************************/

#include <stdlib.h>
#include <cstring>
#include "Game2D.h"

#pragma once

/**
 * CGameState����"��Ϸ״̬"�Ļ���."��Ϸ״̬"Ϊ��Ϸ��ǰ����ʲô.
 * ����״̬������Ϸ״̬ջ��,ջ��״̬Ϊ��ǰ�״̬.
 * ����һ�������࣬Ҫʹ������������
 */

class CGameState
{
friend class StateStack;
protected:
    //ϵͳջ
    CGameState *m_prior;
    //��ʱ��
	bool m_exited;
	//�Ƿ���Ҫ��ǰһ״̬
	bool m_drawPrior;
	//����
    char m_name[100];
public:
    CGameState(const char *name);
    virtual ~CGameState(void);
	
	void virtual Start(CGameState * prior);
    /**
    * �����������, ���뻭ͼ����
    */
    void virtual Draw2D(CGame2D * g2d);

    void virtual PreRender();

	void virtual Draw3D();

    void virtual PostRender();

    /**
    * ���¼�ʱ��
    */
    void virtual Animate(float elapsedTime);

    /**
    * �����������, �������״̬����
    */
	void virtual Update(float elapsedTime);

    bool IsExited() const { return m_exited; }

    const char* GetName() const { return m_name; }

	void virtual DeviceLost() {};
	void virtual DeviceReset() {};
};
