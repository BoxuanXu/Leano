/*================================================================
*   Copyright (C) 2018 . All rights reserved.
*
*   文件名称：epoll_server.h
*   创 建 者： xuboxuan
*   创建日期：2018年04月25日
*   描    述：
*
================================================================*/

#ifndef EPOLL_SERVER_H_
#define EPOLL_SERVER_H_

#include "epoll_event.h"

struct web_event_t;
struct web_connection_t {
  int fd;

  int state;  //当前处理到哪个阶段
  struct web_event_t* read_event;
  struct web_event_t* write_event;
  char* querybuf;
  int query_start_index;  //请求数据的当前指针
  int query_end_index;    //请求数据的下一个位置
  int query_remain_len;   //可用空间

  char method[8];
  char uri[128];
  char version[16];
  char host[128];
  char accept[128];
  char conn[20];
};

struct server {
  int epollfd;
};

void web_epoll_ctl(int epollfd, int ctl, int fd, int flag);
int setnonblocking(int fd);
void initConnection(web_connection_t*& conn);
void web_accept(struct web_connection_t* conn);
void read_request(struct web_connection_t* conn);
void process_request_line(struct web_connection_t* conn);
void process_head(struct web_connection_t* conn);
void process_body(struct web_connection_t* conn);
void send_response(struct web_connection_t* conn);
void try_to_enlarge_buffer(struct web_connection_t& conn);
void empty_event_handler(struct web_connection_t* conn);
void close_conn(struct web_connection_t* conn);

#endif /* SERVER_H_ */
