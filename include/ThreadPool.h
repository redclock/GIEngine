#pragma once
#include <vector>
#include "MyThread.h"

class CThreadPool
{
private:
	std::vector<MyThread *> m_threads;
public:
	void RunThread(MyThread * thread);
	void DeleteThread(MyThread * thread);
	void Clear();
	void DeleteTerminated();
	CThreadPool();
	~CThreadPool();
};