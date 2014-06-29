#pragma once

#include <Windows.h>

class MyThread
{
protected:
	HANDLE m_handle;
	bool m_started;
	bool m_terminating;
	bool m_terminated;
public:
	MyThread();
	virtual ~MyThread();
	
	void Terminate();
	void Start();
	void WaitFor();
	
	bool IsStarted() const { return m_started; }
	bool IsTerminated() const { return m_terminated; }
	bool IsTerminating() const { return m_terminating; }

	virtual void Execute() = 0;

	friend long WINAPI MyThreadProc( long pParam );
};