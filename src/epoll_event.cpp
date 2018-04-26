/*================================================================
*   Copyright (C) 2018 . All rights reserved.
*
*   文件名称：epoll_event.cpp
*   创 建 者： xuboxuan
*   创建日期：2018年04月25日
*   描    述：
*
================================================================*/

#include "include/epoll_event.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <unistd.h>

static int ep = -1;  // epoll对象的描述符,每个进程只有一个

void epoll_init_event(web_connection_t*& conn) { ep = epoll_create(1024); }

/* 添加事件,conn已经设置好回调函数和fd了 */
void epoll_add_event(web_connection_t* conn, int flag) {
  epoll_event ee;
  int fd = conn->fd;
  ee.data.ptr = (void*)conn;
  ee.events = flag;
  epoll_ctl(ep, EPOLL_CTL_ADD, fd, &ee);
}

/* 修改事件,event已经设置好回调函数和fd了 */
void epoll_mod_event(web_connection_t* conn, int flag) {
  epoll_event ee;
  int fd = conn->fd;
  ee.data.ptr = (void*)conn;
  ee.events = flag;
  epoll_ctl(ep, EPOLL_CTL_MOD, fd, &ee);
}

//删除该描述符上的所有事件，若想只删除读事件或写事件，则把相应的事件设置为空函数
void epoll_del_event(web_connection_t* conn) {
  epoll_ctl(ep, EPOLL_CTL_DEL, conn->fd, 0);  //删除事件最后一个参数为0
}
//事件循环函数
int ngx_epoll_process_events() {
  epoll_event event_list[MAX_EVENT_NUMBER];
  while (true) {
    int number = epoll_wait(ep, event_list, MAX_EVENT_NUMBER, -1);
#ifdef DEBUG
    printf("number=%d\n", number);
    printf("当前进程ID为： %d \n", getpid());
#endif
    int i;
    for (i = 0; i < number; i++) {
      web_connection_t* conn = (web_connection_t*)(event_list[i].data.ptr);
      int socket = conn->fd;  //当前触发的fd
      //读事件
      if (event_list[i].events & EPOLLIN) {
        conn->read_event->handler(conn);
      }
      //写事件
      else if (event_list[i].events & EPOLLOUT) {
        conn->write_event->handler(conn);
      } else if (event_list[i].events & EPOLLERR) {
      }
    }
  }
  return 0;
}
