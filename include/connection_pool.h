/*================================================================
*   Copyright (C) 2018 . All rights reserved.
*
*   文件名称：te.h
*   创 建 者：  xuboxuan
*   创建日期：2018年03月29日
*   描    述：
*
================================================================*/

#ifndef _CONNECTION_POOL_H
#define _CONNECTION_POOL_H

#include <cppconn/connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/metadata.h>
#include <cppconn/parameter_metadata.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/sqlstring.h>
#include <cppconn/statement.h>
#include <mysql_connection.h>
#include <mysql_driver.h>
#include <pthread.h>
#include <list>

#include "common.h"

using namespace std;
using namespace sql;

class ConnPool {
 private:
  int curSize;  //当前已建立的数据库连接数量
  int maxSize;  //连接池中定义的最大数据库连接数
  string username;
  string password;
  string url;
  pthread_mutex_t lock;  //线程锁
  static ConnPool *connPool;
  Driver *driver;

  Connection *CreateConnection();            //创建一个连接
  void InitConnection(int iInitialSize);     //初始化数据库连接池
  void DestoryConnection(Connection *conn);  //销毁数据库连接对象
  void DestoryConnPool();                    //销毁数据库连接池
  ConnPool(string url, string user, string password, int maxSize);  //构造方法
 public:
  ~ConnPool();
  Connection *GetConnection();  //获得数据库连接
  list<Connection *> connList;  //连接池的容器队列
  void ReleaseConnection(Connection *conn);  //将数据库连接放回到连接池的容器中
  static ConnPool *GetInstance(int maxSize = 2,
                               string url = "tcp://127.0.0.1:3306",
                               string user = "root",
                               string pwd = "root");  //获取数据库连接池对象
};
#endif /*_CONNECTION_POOL_H */
