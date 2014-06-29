#include "ThreadPool.h"

CThreadPool::CThreadPool()
{

}

CThreadPool::~CThreadPool()
{
	Clear();
}

void CThreadPool::RunThread(MyThread *thread)
{
	thread->Start();
	m_threads.push_back(thread);
}

void CThreadPool::DeleteThread(MyThread * thread)
{
	for (std::vector<MyThread *>::iterator itr = m_threads.begin(); itr != m_threads.end(); itr++)
	{
		if (*itr == thread)
		{
			delete thread;
			if (itr == m_threads.end() - 1)
			{
				m_threads.pop_back();
				break;
			}else
			{
				m_threads.erase(itr);
				break;
			}
		}
	}
}


void CThreadPool::DeleteTerminated()
{
	for (std::vector<MyThread *>::iterator itr = m_threads.begin(); itr != m_threads.end(); itr++)
	{
		if ((*itr)->IsTerminated())
		{
			delete (*itr);
			if (itr == m_threads.end() - 1)
			{
				m_threads.pop_back();
				break;
			}else if (itr == m_threads.begin())
			{
				m_threads.erase(itr);
				itr = m_threads.begin();
				break;
			}else
			{
				itr --;
				m_threads.erase(itr + 1);
			}

		}
	}
}

void CThreadPool::Clear()
{
	for (size_t i = 0; i < m_threads.size(); i++)
	{
		m_threads[i]->Terminate();
	}

	for (size_t i = 0; i < m_threads.size(); i++)
	{
		delete m_threads[i];
	}

	m_threads.clear();
}
