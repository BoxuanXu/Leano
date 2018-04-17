/*================================================================
*   Copyright (C) 2018 . All rights reserved.
*   
*   文件名称：te.h
*   创 建 者：  xuboxuan
*   创建日期：2018年03月29日
*   描    述：
*
================================================================*/
#ifndef _COMMON_H
#define _COMMON_H
#include <string.h>
#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>

#include "logging.hpp"

#include <unistd.h>
#include <assert.h>

using namespace std;

int32_t myexec(const char *cmd, std::vector<std::string> &resvec); 
#endif /*_COMMON_H*/
