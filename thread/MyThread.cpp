#include "MyThread.h"

long WINAPI  MyThreadProc( long pParam )
{
	MyThread * t = (MyThread*) pParam;
	t->m_started = true;
	t->Execute();
	t->m_terminated = true;
	t->m_terminating = false;
	return 0;
}

MyThread::MyThread()
{
	m_started = false;
	m_terminated = false;
	m_terminating = false;
	m_handle = NULL;
}

MyThread::~MyThread()
{
	if (m_started && !m_terminating && !m_terminated)
	{
		Terminate();
	}
	if (m_terminating)
	{
		WaitFor();
	}
}

void MyThread::Start()
{
	m_handle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)MyThreadProc, this, 0, NULL);
}

void MyThread::Terminate()
{
	m_terminating = true;
}

void MyThread::WaitFor()
{
	if (!m_started || m_terminated) return;
	WaitForSingleObject(m_handle, INFINITE);
}
