/*================================================================
*   Copyright (C) 2018 . All rights reserved.
*   
*   文件名称：conditionqueue.h
*   创 建 者：  xuboxuan
*   创建日期：2018年04月14日
*   描    述：
*
================================================================*/


#pragma once
#include <queue>
#include <pthread.h>
#include <functional>
#include "common.h"

using std::queue;

using ReleaseHandler = std::function<void(void* data)>;


template <class T>
class ConditionQueue
{
private:
	pthread_mutex_t m_mutex;
	pthread_cond_t m_cond;
	queue<T *> m_dataqueue;
	unsigned int m_used;

	unsigned int wait_thread_num;
public:
	ConditionQueue()
	{
		pthread_cond_init(&m_cond, NULL);
		pthread_mutex_init(&m_mutex, NULL);
		m_used = 0;
		wait_thread_num = 0;
	}
	
	~ConditionQueue()
	{
		CleanQueue();
		pthread_mutex_destroy(&m_mutex);
		pthread_cond_destroy(&m_cond);
	}

	unsigned int wait_size()
	{
		return wait_thread_num;
	}

	
	unsigned int size()
	{
		unsigned int nret = 0;
		pthread_mutex_lock(&m_mutex);
		nret = m_used + m_dataqueue.size();		
		pthread_mutex_unlock(&m_mutex);
		return nret;
	} 
	
	unsigned int decrease()
	{
		unsigned int nret = 0;
		pthread_mutex_lock(&m_mutex);
		if(m_used > 0)
			--m_used;
		nret = m_used;
		pthread_mutex_unlock(&m_mutex);
		return nret;
	}	

	unsigned int increase()
	{
		unsigned int nret = 0;
		pthread_mutex_lock(&m_mutex);
		++m_used;
		nret = m_used;
		pthread_mutex_unlock(&m_mutex);
		return nret;
	}

	void CleanQueue()
	{
		pthread_mutex_lock(&m_mutex);
		T *ptr = nullptr;
		while(!m_dataqueue.empty())
		{
			ptr = m_dataqueue.front();
			m_dataqueue.pop();
			if(ptr)
				delete ptr;
		}
		m_used = 0;
		pthread_mutex_unlock(&m_mutex);
	}

	void CleanQueue(ReleaseHandler rel_handler)
	{
		pthread_cond_broadcast(&(m_cond));
		pthread_mutex_lock(&m_mutex);
		T *ptr = nullptr;
		while(!m_dataqueue.empty())
		{
			ptr = m_dataqueue.front();
			m_dataqueue.pop();
			if(ptr)
				rel_handler(ptr);
		}
		m_used = 0;
		pthread_mutex_unlock(&m_mutex);
	}

	bool empty()
	{
		bool bret=false;
		pthread_mutex_lock(&m_mutex);
		bret = m_dataqueue.empty();
		pthread_mutex_unlock(&m_mutex);
		return bret;
	}


	void push(T *data)
	{
		pthread_mutex_lock(&m_mutex);
		m_dataqueue.push(data);
		pthread_mutex_unlock(&m_mutex);
		pthread_cond_signal(&m_cond);
	}

	T* try_pop()
	{
		T *ptr = nullptr;
		pthread_mutex_lock(&m_mutex);
		if(!m_dataqueue.empty())
		{
			ptr = m_dataqueue.front();
			m_dataqueue.pop();
			++m_used;
		}
		pthread_mutex_unlock(&m_mutex);
		return ptr;
	}

	T * wait_pop()
	{
		
		T *ptr = nullptr;
		pthread_mutex_lock(&m_mutex);
		while(m_dataqueue.empty())
		{
			LEANO_LOG_INFO << "thread " << (long long unsigned int)pthread_self() << " is waiting\n";
			++wait_thread_num;
			pthread_cond_wait(&m_cond, &m_mutex); 
       		}
		
		--wait_thread_num;
		if(!m_dataqueue.empty())
       		{
           		ptr = m_dataqueue.front();
           		m_dataqueue.pop();
		   	++m_used;
       		}
		pthread_mutex_unlock(&m_mutex);
		return ptr;
		
	}

	T * wait_timeout_pop(size_t timeout)
	{
		struct timespec ntimeout;
		ntimeout.tv_sec = timeout;
		ntimeout.tv_nsec = 0;
		
		T *ptr = nullptr;
		pthread_mutex_lock(&m_mutex);
		while(m_dataqueue.empty())
		{
			if(pthread_cond_timedwait(&m_cond, &m_mutex, &ntimeout) != 0)
			{
				pthread_mutex_unlock(&m_mutex);
				return ptr;
			}
       		}
		
		if(!m_dataqueue.empty())
       		{
           		ptr = m_dataqueue.front();
           		m_dataqueue.pop();
		   	++m_used;
       		}
		pthread_mutex_unlock(&m_mutex);
		return ptr;
		
	}
};
