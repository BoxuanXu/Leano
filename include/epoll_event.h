/*================================================================
*   Copyright (C) 2018  All rights reserved.
*
*   文件名称：epoll_event.h
*   创 建 者： xuboxuan
*   创建日期：2018年04月25日
*   描    述：
*
================================================================*/

#ifndef EVENT_H_
#define EVENT_H_

#include <netinet/in.h>
#include "server.h"
#define MAX_EVENT_NUMBER 10000
#define QUERY_INIT_LEN 1024
#define REMAIN_BUFFER 512

/* 以下是处理机的状态 */
#define ACCEPT 1
#define READ 2
#define QUERY_LINE 4
#define QUERY_HEAD 8
#define QUERY_BODY 16
#define SEND_DATA 32

struct web_connection_t;

typedef void (*event_handler_pt)(web_connection_t* conn);

//每一个事件都由web_event_t结构体来表示
struct web_event_t {
  /*为1时表示事件是可写的，通常情况下，它表示对应的TCP连接目前状态是可写的，也就是连接处于可以发送网络包的状态*/
  unsigned write : 1;
  /*为1时表示此事件可以建立新的连接,通常情况下，在ngx_cycle_t中的listening动态数组中，每一个监听对象ngx_listening_t对应的读事件中
的accept标志位才会是1*/
  unsigned accept : 1;
  //为1时表示当前事件是活跃的，这个状态对应着事件驱动模块处理方式的不同，例如：在添加事件、删除事件和处理事件时，该标志位的不同都会对应着不同的处理方式
  unsigned active : 1;
  unsigned oneshot : 1;
  unsigned eof : 1;    //为1时表示当前处理的字符流已经结束
  unsigned error : 1;  //为1时表示事件处理过程中出现了错误

  event_handler_pt handler;  //事件处理方法，每个消费者模块都是重新实现它
  unsigned closed : 1;  //为1时表示当前事件已经关闭
};

void epoll_init_event(web_connection_t*& conn);
void epoll_add_event(web_connection_t* conn, int flag);
void epoll_mod_event(web_connection_t* conn, int flag);
void epoll_del_event(web_connection_t* conn);
int ngx_epoll_process_events();
#endif /* EVENT_H_ */
