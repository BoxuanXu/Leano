/*================================================================
*   Copyright (C) 2018 . All rights reserved.
*
*   文件名称：common.cpp
*   创 建 者：  xuboxuan
*   创建日期：2018年04月04日
*   描    述：
*
================================================================*/

#include "include/common.h"

int32_t myexec(const char *cmd, std::vector<std::string> &resvec) {
  resvec.clear();
  FILE *pp = popen(cmd, "r");  //建立管道
  if (!pp) {
    return -1;
  }

  int res = 0;
  try {
    char tmp[1024];  //设置一个合适的长度，以存储每一行输出
    while (fgets(tmp, sizeof(tmp), pp) != NULL) {
      if (tmp[strlen(tmp) - 1] == '\n') {
        tmp[strlen(tmp) - 1] = '\0';  //去除换行符
      }
      resvec.push_back(tmp);
      LEANO_LOG_INFO << tmp;
      string tmp_s = tmp;
      int idx = tmp_s.find("convert to res success");
      if (idx != string::npos)  //存在
        res = 1;

#ifdef DEBUG
      idx = tmp_s.find("progress:");
      if (idx != string::npos)  //存在
      {
        string progress = tmp_s.substr(idx + 10, tmp_s.length() - idx - 10);
        LEANO_LOG_INFO << "progress:" << progress;
      }
#endif
    }
    pclose(pp);  //关闭管道

  } catch (...) {
    res = -1;
    pclose(pp);  //关闭管道
  }
  return res;
}
