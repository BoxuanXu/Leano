/*================================================================
*   Copyright (C) 2018 . All rights reserved.
*
*   文件名称：te.h
*   创 建 者：  xuboxuan
*   创建日期：2018年03月29日
*   描    述：
*
================================================================*/
#include "include/thread_pool.h"

ThreadPool* ThreadPool::m_instance = new ThreadPool;

ThreadPool* T_Pool = ThreadPool::getInstance();

ThreadPool::ThreadPool() {
  pool = new CPool();
  pthread_mutex_init(&(pool->queue_lock), NULL);
  pthread_cond_init(&(pool->queue_ready), NULL);
}

ThreadPool::~ThreadPool() {
  if (pool != NULL) {
    delete pool;
    pool = NULL;
  }

  if (threadid != NULL) {
    delete[] threadid;
    threadid = NULL;
  }
}

bool ThreadPool::start(int max_thread_num) {
  pool->max_thread_num = max_thread_num;
  pool->cur_queue_size = 0;
  pool->shutdown = 0;

  threadid = new pthread_t[max_thread_num];

  for (size_t i = 0; i < max_thread_num; ++i) {
    pthread_create(&threadid[i], NULL, thread_routine, NULL);
  }

  return true;
}

int ThreadPool::pool_add_worker(void (*process)(void* arg), void* ptr) {
  CMsg* newMsg = new CMsg();  // release in thread_routine function
  newMsg->process = process;
  newMsg->arg = ptr;

  pthread_mutex_lock(&(pool->queue_lock));
  pool->queue_b.push(newMsg);
  pool->cur_queue_size++;

  pthread_mutex_unlock(&(pool->queue_lock));
  pthread_cond_signal(&(pool->queue_ready));
  return 0;
}

int ThreadPool::pool_destory() {
  if (pool->shutdown == 1) return -1;
  pool->shutdown = 1;

  sleep(3);
  pthread_cond_broadcast(&(pool->queue_ready));
  for (size_t i = 0; i < pool->max_thread_num; ++i) {
    pthread_cond_broadcast(&(pool->queue_ready));
    pthread_join(threadid[i], NULL);
  }

  delete[] threadid;

  for (size_t i = 0; i < pool->queue_b.size(); ++i) {
    delete (pool->queue_b.front());
    pool->queue_b.pop();
  }

  pthread_mutex_destroy(&(pool->queue_lock));
  pthread_cond_destroy(&(pool->queue_ready));

  delete pool;
  pool = NULL;
  return 0;
}

static void* thread_routine(void* arg) {
#ifdef DEBUG
  LEANO_LOG_INFO << "start threading" << (long long unsigned int)pthread_self()
                 << "...................\n";
#endif
  while (1) {
    pthread_mutex_lock(&(T_Pool->pool->queue_lock));
    while (T_Pool->pool->cur_queue_size == 0 && !T_Pool->pool->shutdown) {
#ifdef DEBUG
      LEANO_LOG_INFO << "thread " << (long long unsigned int)pthread_self()
                     << " is waiting\n";
      pthread_cond_wait(&(T_Pool->pool->queue_ready),
                        &(T_Pool->pool->queue_lock));
#endif
    }

    if (T_Pool->pool->shutdown) {
      pthread_mutex_unlock(&(T_Pool->pool->queue_lock));
#ifdef DEBUG
      LEANO_LOG_INFO << "thread " << (long long unsigned int)pthread_self()
                     << " will exit\n";
#endif
      pthread_exit(NULL);
    }

    //#ifdef DEBUG
    //		LEANO_LOG_INFO << "thread " << (long long unsigned int)pthread_self()
    //<< " is start to work....\n";
    //#endif

    if (T_Pool->pool->cur_queue_size > 0) {
      T_Pool->pool->cur_queue_size--;

      CMsg* q_msg = T_Pool->pool->queue_b.front();
      T_Pool->pool->queue_b.pop();

      pthread_mutex_unlock(&(T_Pool->pool->queue_lock));
      (*(q_msg->process))(q_msg->arg);
      if (q_msg != NULL) {
        delete (q_msg);
        q_msg = NULL;
      }
    }
  }

  pthread_exit(NULL);
}
