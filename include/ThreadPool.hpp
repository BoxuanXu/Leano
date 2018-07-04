/*================================================================
*   Copyright (C) 2018 xuboxuan. All rights reserved.
*
*   文件名称：ThreadPool.h
*   创 建 者： xuboxuan
*   创建日期：2018年07月02日
*   描    述：
*
================================================================*/

#ifndef _THREADPOOL_H
#define _THREADPOOL_H
#include <signal.h>
#include <functional>
#include <list>
#include <memory>
#include <thread>
#include "SyncQueue.hpp"
#include "common.h"
const int MaxTaskCount = 99;

template <typename T>
class ThreadPool {
 public:
  using Task = std::function<void(T param)>;
  ThreadPool(int numThreads = std::thread::hardware_concurrency()) : m_queue(MaxTaskCount) {
    if (numThreads > 99) numThreads = MaxTaskCount;
    Start(numThreads);
  }

  ~ThreadPool(void) {
    //如果没有停止时则主动停止线程池
    Stop();
  }

  void Stop() {
    std::call_once(m_flag,
                   [this] { StopThreadGroup(); });  //保证多线程情况下只调用一次StopThreadGroup
  }

  /*void AddTask(Task&&task)
  {
      m_queue.Put(std::forward<Task>(task));
  }

  void AddTask(const Task& task)
  {
      m_queue.Put(task);
  }*/

  void AddData(const T& task_param) { m_queue.Put(task_param); }

  void SetProcessFun(Task pfun) { _pfun = pfun; }

  //主线程调用: t_pool->Pause();sleep(30);t_pool->Continues();
  void Pause() { pause(); };

  void Continues() { continues(); }

 private:
  void Start(int numThreads) {
    m_running = true;
    //创建线程组
    for (int i = 0; i < numThreads; ++i) {
      m_threadgroup.push_back(std::make_shared<std::thread>(&ThreadPool::RunInThread, this));
    }

    for (auto thread : m_threadgroup)  //等待线程结束
    {
      if (thread) {
        waitsig_debug_group[thread->get_id()] = false;
      }
    }
  }

  void pause() {
    LEANO_LOG_INFO << m_threadgroup.size() << " thread be asked to pause.";
    //此处暂停，但是需要等待线程处理完当前param，才能进入while循环去查询是否wait,如果wait的时间在线程处理完当前param前就结束了，则线程无变化
    //所以如果每个线程刚好只分到一个参数，这部分代码并不起作用，只用于队列中参数多于线程数的情况
    for (auto thread : m_threadgroup) 
    {
      if (thread) {
        waitsig_debug_group[thread->get_id()] = true;
      }
    }
  }

  void continues() {
    //此处恢复暂停，但是需要等待线程处理完当前param，才能进入while循环去查询是否恢复,如果wait的时间在线程处理完当前param前就结束了，则线程无变化
    LEANO_LOG_INFO << "Thread be asked to continues.";
    for (auto thread : m_threadgroup)  //等待线程结束
    {
      if (thread) {
        pthread_t tid = thread->native_handle();
        pthread_kill(tid, SIGUSR1);
        waitsig_debug_group[thread->get_id()] = false;
      }
    }
  }

  void RunInThread() {
    // set signal to stop or run
    sigset_t sig;
    sigemptyset(&sig);
    sigaddset(&sig, SIGUSR1);
    pthread_sigmask(SIG_BLOCK, &sig, nullptr);

    sigset_t waitsig_;
    sigemptyset(&waitsig_);
    sigaddset(&waitsig_, SIGUSR1);

    // set thread init stop , then main thread continues;
    int sig_num;
    //这段注释用于让线程只初始化，然后在此次阻塞，直到主线程调用Continue函数.
    /*
    LEANO_LOG_INFO << "thread " <<  std::this_thread::get_id() << " has been init!!!";

    sigwait(&waitsig_, &sig_num);

    LEANO_LOG_INFO << "thread: " << std::this_thread::get_id() << " begin run.";
    */

    while (m_running) {
      //此处暂停，但是需要等待线程处理完当前param，才能进入while循环去查询是否wait,如果wait的时间在线程处理完当前param前就结束了，则线程无变化
      if (waitsig_debug_group[std::this_thread::get_id()]) {
        LEANO_LOG_INFO << "thread " << std::this_thread::get_id() << " is waiting..."; 
        int ret = sigwait(&waitsig_, &sig_num);
        if (EINTR == ret) {
          continue;
        } else if (ret) {
          m_running = false;
          break;
        }
        LEANO_LOG_INFO << "thread " << std::this_thread::get_id() << " finish waiting..."; 
      }
      //取任务分别执行
      // std::list<Task> list;

      T task_param;

      m_queue.Take(task_param);

      if (!m_running) return;

      LEANO_LOG_INFO << "thread : " << std::this_thread::get_id() << " begin process";

      _pfun(task_param);
      /*for (auto& task : list)
      {
          if (!m_running)
              return;

          task();
      }*/
    }
  }

  void StopThreadGroup() {
    LEANO_LOG_INFO << "ask to terminal...";
    m_queue.Stop();     //让同步队列中的线程停止
    m_running = false;  //置为false，让内部线程跳出循环并退出

    for (auto thread : m_threadgroup)  //等待线程结束
    {
      if (thread) thread->join();
    }
    m_threadgroup.clear();
  }

  std::list<std::shared_ptr<std::thread>> m_threadgroup;  //处理任务的线程组
  SyncQueue<T> m_queue;                                   //同步队列
  std::once_flag m_flag;
  Task _pfun;
  std::map<std::thread::id, bool> waitsig_debug_group;
};
#endif  // THREADPOOL_H
