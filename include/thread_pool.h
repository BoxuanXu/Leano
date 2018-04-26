/*================================================================
*   Copyright (C) 2018 . All rights reserved.
*
*   文件名称：te.h
*   创 建 者：  xuboxuan
*   创建日期：2018年03月29日
*   描    述：
*
================================================================*/
#ifndef _THREAD_POOL_H
#define _THREAD_POOL_H
#include <assert.h>
#include <pthread.h>
#include <queue>
#include "Msg.h"
#include "common.h"
#include "config.h"

typedef struct Pool {
  pthread_mutex_t queue_lock;
  pthread_cond_t queue_ready;
  queue<CMsg*> queue_b;
  int shutdown = 0;
  int max_thread_num;
  int cur_queue_size;

} CPool;

class ThreadPool {
 public:
  static ThreadPool* getInstance() { return m_instance; }

  ~ThreadPool();
  bool start(int max_thread_num = 1);

  int pool_add_worker(void (*process)(void* arg), void* ptr);

  int pool_destory();

  CPool* pool = NULL;

 private:
  ThreadPool();

  static ThreadPool* m_instance;

  pthread_t* threadid;

  Config* pcfg = Config::get_instance();
};

static void* thread_routine(void* arg);

extern ThreadPool* T_Pool;
#endif /*_THREAD_POOL_H */
