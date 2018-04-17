/*================================================================
*   Copyright (C) 2018 . All rights reserved.
*   
*   文件名称：te.h
*   创 建 者：  xuboxuan
*   创建日期：2018年03月29日
*   描    述：
*
================================================================*/
#ifndef _MSG_H
#define _MSG_H

typedef struct download_param
{
   string file_path;
   string download_dir;
}Download_Param;

typedef struct msg
{
   void (*process)(void* arg);
   void *arg;
}CMsg;

#endif  /*_THREAD_POOL_H */
