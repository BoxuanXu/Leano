/*================================================================
*   Copyright (C) 2018 . All rights reserved.
*
*   文件名称：epoll_server.cpp
*   创 建 者： seetatech & xuboxuan
*   创建日期：2018年04月25日
*   描    述：
*
================================================================*/

#include "include/epoll_server.h"

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <iostream>

#include <sys/types.h>
#include <unistd.h>
using namespace std;

#define MAXBUF 1024
#define EPOLL_HELLO "epoll hello!"
/* 定义常量 */
#define HTTP_DEF_PORT 80      /* 连接的缺省端口 */
#define HTTP_BUF_SIZE 1024    /* 缓冲区的大小 */
#define HTTP_FILENAME_LEN 256 /* 文件名长度 */
char* http_res_hdr_tmpl =
    "HTTP/1.1 200 OK\r\nServer: Xubx's Server <0.1>\r\n"
    "Accept-Ranges: bytes\r\nContent-Length: %d\r\nConnection: Close\r\n"
    "Content-Type: %s\r\n\r\n";

int filefd = 0;

void http_send_response(struct web_connection_t* conn);
int main(int argc, char* argv[]) {
  const char* ip = "127.0.0.1";
  int port = 12347;

  signal(SIGPIPE,
         SIG_IGN);  //原因：http://blog.sina.com.cn/s/blog_502d765f0100kopn.html

  int listenfd = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in address;
  bzero(&address, sizeof(address));
  address.sin_family = AF_INET;
  inet_pton(AF_INET, ip, &address.sin_addr);
  address.sin_port = htons(port);
  bind(listenfd, (struct sockaddr*)&address, sizeof(address));
  listen(listenfd, 50);

  web_connection_t* conn = NULL;
  epoll_init_event(conn);
  initConnection(conn);  //创建一个用于接受连接的结构体
  if (conn == NULL) {
    printf("---创建监听结构体失败---\n");
    return -1;
  };  //创建监听结构体

  conn->fd = listenfd;
  conn->read_event->handler = web_accept;
  epoll_add_event(conn, EPOLLIN | EPOLLERR);

  setnonblocking(listenfd);

  filefd = open("../http/hello.html", O_RDONLY);

  pid_t pid = fork();

  if (pid == 0) {
    ngx_epoll_process_events();  //进入事件循环，等待事件到达
  } else {
    while (1) {
      sleep(1);
    }
  }
  close(listenfd);
}
void initConnection(web_connection_t*& conn) {
  conn = (web_connection_t*)malloc(sizeof(web_connection_t));
  conn->read_event = (web_event_t*)malloc(sizeof(web_event_t));
  conn->write_event = (web_event_t*)malloc(sizeof(web_event_t));
  conn->state = ACCEPT;

  conn->querybuf = (char*)malloc(QUERY_INIT_LEN);
  if (!conn->querybuf) {
    printf(" malloc error\n");
    return;
  }
  conn->query_start_index = 0;
  conn->query_end_index = 0;
  conn->query_remain_len = QUERY_INIT_LEN;
}

int setnonblocking(int fd) {
  // int old_option = fcntl(fd,F_GETFL);
  int old_option = fcntl(fd, F_GETFD, 0);
  int new_option = old_option | O_NONBLOCK;
  fcntl(fd, F_SETFL, new_option);
  return old_option;
}

void web_accept(web_connection_t* conn) {
#ifdef DEBUG
  printf("-----------accept-------\n");
#endif
  struct sockaddr* client_address;
  socklen_t client_addrlength = sizeof(client_address);
  int connfd =
      accept(conn->fd, (struct sockaddr*)&(client_address), &client_addrlength);
  if (connfd == -1) {
    printf("accept error\n");
    return;
  }
  web_connection_t* new_conn = NULL;
  initConnection(new_conn);  //创建一个新的连接结构体
  if (new_conn == NULL) {
    printf("---创建连接结构体失败---\n");
    return;
  };

  new_conn->fd = connfd;
  new_conn->state = READ;
  new_conn->read_event->handler = read_request;
  epoll_add_event(new_conn, EPOLLIN | EPOLLERR | EPOLLET);
  // epoll_add_event(new_conn,EPOLLIN | EPOLLERR);

  setnonblocking(connfd);
}
/* 读取 http 请求，仅读出未正常解析 */

void read_request(struct web_connection_t* conn) {
#ifdef DEBUG
  printf("-----------read_begin-------\n");
#endif
  int len, fd = conn->fd;
  while (true) {
    /* 尝试增加缓冲区空间 */
    try_to_enlarge_buffer(*conn);
    len = recv(fd, conn->querybuf + conn->query_end_index,
               conn->query_remain_len, 0);
    if (len < 0) {
#ifdef DEBUG
      printf("-----数据读取完毕-----\n");
#endif
      break;  //表示当前数据读取完毕，不是出错
    } else if (len > 0) {
      conn->query_end_index += len;
      conn->query_remain_len -= len;
    } else if (len == 0) {
#ifdef DEBUG
      printf("----连接关闭-----\n");
#endif
      epoll_del_event(conn);
      close_conn(conn);
      return;
    }
  }
#ifdef DEBUG
  cout << "-----客户端的内容是:" << endl;
  cout << conn->querybuf << endl;

#endif
  process_request_line(conn);

  return;
}
void process_request_line(struct web_connection_t* conn) {
  int len;
  char* ptr = strpbrk(conn->querybuf + conn->query_start_index, " \t");
  if (!ptr) {
#ifdef DEBUG
    printf("请求行解析失败\n");
#endif
    return;
  }
  len = ptr - conn->querybuf - conn->query_start_index;
  strncpy(conn->method, conn->querybuf + conn->query_start_index, len);
#ifdef DEBUG
  cout << "metnod=" << conn->method << endl;
#endif
  conn->query_start_index += (len + 1);
  ptr = strpbrk(conn->querybuf + conn->query_start_index, " \t");
  if (!ptr) {
    printf("请求行解析失败\n");
    return;
  }
  len = ptr - conn->querybuf - conn->query_start_index;
  strncpy(conn->uri, conn->querybuf + conn->query_start_index, len);
#ifdef DEBUG
  cout << "uri=" << conn->uri << endl;
#endif
  conn->query_start_index += (len + 1);
  ptr = strpbrk(conn->querybuf, "\n");  //先是回车\r,再是换行\n
  if (!ptr) {
    printf("请求行解析失败\n");
    return;
  }
  len = ptr - conn->querybuf - conn->query_start_index;
  strncpy(conn->version, conn->querybuf + conn->query_start_index, len);
#ifdef DEBUG
  cout << "version=" << conn->version << endl;
#endif
  conn->query_start_index += (len + 1);

#ifdef DEBUG
  cout << "-----请求行解析完毕----------" << endl;
#endif
  process_head(conn);
  /*conn->state = SEND_DATA;
  conn->write_event->handler = send_response;
  conn->read_event->handler = empty_event_handler;//读事件回调函数设置为空
  epoll_mod_event(conn,EPOLLOUT | EPOLLERR);*/
}

void process_head(struct web_connection_t* conn) {
#ifdef DEBUG
  cout << "-------开始解析首部------" << endl;
#endif
  char* end_line;
  int len;

  while (true) {
    end_line = strpbrk(conn->querybuf + conn->query_start_index, "\n");
    len = end_line - conn->querybuf - conn->query_start_index;
    if (len == 1) {
#ifdef DEBUG
      printf("解析完毕\n");
#endif
      conn->query_start_index += (len + 1);
#ifdef DEBUG
      cout << conn->querybuf + conn->query_start_index << endl;
#endif
      break;
    } else {
      if (strncasecmp(conn->querybuf + conn->query_start_index, "Host:", 5) ==
          0) {
        strncpy(conn->host, conn->querybuf + conn->query_start_index + 6,
                len - 6);
#ifdef DEBUG
        cout << "host=" << conn->host << endl;
#endif
      } else if (strncasecmp(conn->querybuf + conn->query_start_index,
                             "Accept:", 7) == 0) {
        strncpy(conn->accept, conn->querybuf + conn->query_start_index + 8,
                len - 8);
#ifdef DEBUG
        cout << "accept=" << conn->accept << endl;
#endif
      } else if (strncasecmp(conn->querybuf + conn->query_start_index,
                             "Connection:", 11) == 0) {
        strncpy(conn->conn, conn->querybuf + conn->query_start_index + 12,
                len - 12);
#ifdef DEBUG
        cout << "connection=" << conn->conn << endl;
#endif
      } else {
      }
      conn->query_start_index += (len + 1);
    }
  }
  process_body(conn);
#ifdef DEBUG
  printf("----首部解析完毕----------\n");
#endif
}
void process_body(struct web_connection_t* conn) {
  if (conn->query_start_index == conn->query_end_index) {
#ifdef DEBUG
    printf("---包体为空----\n");
#endif
  } else {
#ifdef DEBUG
    printf("---丢体包体-----\n");
#endif
  }
  conn->query_start_index = conn->query_end_index = 0;

  conn->state = SEND_DATA;
  conn->write_event->handler = send_response;
  conn->read_event->handler = empty_event_handler;  //读事件回调函数设置为空
  // epoll_mod_event(conn,EPOLLOUT | EPOLLERR | EPOLLET);
  epoll_mod_event(conn, EPOLLOUT | EPOLLERR);
}
/* 向客户端发送 HTTP 响应 */
void send_response(struct web_connection_t* conn) {
  /*char path[128] = "http";//根目录下的文件夹
  int len = strlen(conn->uri);
  memcpy(path+4,conn->uri,len);
  len += 4;
  path[len] = '\0';//很重要

  int filefd1 = open(path,O_RDONLY);
  if(filefd1 < 0)
  {
          cout << filefd1 << ":无法打开该文件" <<endl;
          return ;
  }
  struct stat stat_buf;
  fstat(filefd1,&stat_buf);*/
  char read_buf[HTTP_BUF_SIZE];
  char http_header[1024];
  int read_len = strlen(EPOLL_HELLO);
  int hdr_len = sprintf(http_header, http_res_hdr_tmpl, read_len, "text/html");
  int send_len = send(conn->fd, http_header, hdr_len, 0);
  if (send_len < 0) {
    printf("send header Error\n");
    return;
  }
  /*static char buf[1024];
  memset(buf, '\0',sizeof(buf));
  FILE* fp = fopen("http/hello.html", "r");
  if(read(filefd1, buf, sizeof(buf)))
  {
          printf("buf:%s\n",buf);
  }*/
  // read_len = fread(read_buf, sizeof(char), HTTP_BUF_SIZE, res_file);
  read_len = strlen(EPOLL_HELLO);
  memset(read_buf, 0, HTTP_BUF_SIZE);
  memcpy(read_buf, EPOLL_HELLO, read_len);

  if (read_len > 0) {
    send_len = send(conn->fd, read_buf, read_len, 0);
    // file_len -= read_len;
  }

  // sendfile(conn->fd,filefd1,NULL,stat_buf.st_size);
  // close(filefd1);

  // close(conn->fd);//如果不关闭该连接socket，则浏览器一直在加载，如何解决，保持keep-alive?

  // conn->state = READ;
  // conn->read_event->handler = read_request;
  // epoll_mod_event(conn,EPOLLIN | EPOLLERR);
  epoll_del_event(conn);
  close_conn(conn);
  return;
  // sleep(2);
}

void try_to_enlarge_buffer(struct web_connection_t& conn) {
  if (conn.query_remain_len < REMAIN_BUFFER) {
    int new_size = strlen(conn.querybuf) + QUERY_INIT_LEN;
    conn.querybuf = (char*)realloc(conn.querybuf, new_size);
    conn.query_remain_len = new_size - conn.query_end_index;
  }
}
void empty_event_handler(struct web_connection_t* conn) {}
//关闭一个连接
void close_conn(struct web_connection_t* conn) {
  static int count = 0;
  count++;
#ifdef DEBUG
  printf("关闭第%d个连接\n", count);
#endif
  close(conn->fd);
  free(conn->querybuf);
  free(conn->read_event);
  free(conn->write_event);
  free(conn);
}
