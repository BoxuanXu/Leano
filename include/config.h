/*================================================================
*   Copyright (C) 2018 . All rights reserved.
*   
*   文件名称：config.h
*   创 建 者：  xuboxuan
*   创建日期：2018年04月02日
*   描    述：
*
================================================================*/


#ifndef _CONFIG_H
#define _CONFIG_H

#include <string>
#include <vector>
#include "aocsingleton.hpp"

//#ifdef WITH_FILECONF
#include <json/json.h>
//#endif
#include <unistd.h>
#include "common.h"
namespace {
static const char* TASK_SHUFFLE = "shuffle";
static const char* THREAD_NUM = "thread_num";
static const char* DBDATA_HOST = "dbdata_host";
static const char* DBDATA_USER = "dbdata_user";
static const char* DBDATA_PWD = "dbdata_pwd";
static const char* DBTASK_HOST = "dbtask_host";
static const char* DBTASK_USER = "dbtask_user";
static const char* DBTASK_PWD = "dbtask_pwd";
}  // namespace

typedef struct {
    std::string Fid;
    std::string TaskID;
    std::string EngineType;
    std::string TaskType;
    int Task_Shuffle;
    int Thread_Num;
    std::string Server;
    std::string Ceph_path;
    std::string DBData_Host;
    std::string DBData_User;
    std::string DBData_Pwd;
    std::string DBTask_Host;
    std::string DBTask_User;
    std::string DBTask_Pwd;
} FdConf;

class Config : public IAocSingleton<Config> {
   public:
    bool init(const std::string& cfg_path);

    const std::string get_Fid() const { return cfg_.Fid; }
    const std::string get_TaskID() const { return cfg_.TaskID; }
    const std::string get_EngineType() const { return cfg_.EngineType; }
    const std::string get_TaskType() const { return cfg_.TaskType; }
    const int get_Task_Shuffle() const { return cfg_.Task_Shuffle; }
    const int get_Thread_Num() const { return cfg_.Thread_Num; }
    const std::string get_Server() const { return cfg_.Server; }
    const std::string get_Ceph_path() const { return cfg_.Ceph_path; }
    const std::string get_DBData_Host() const { return cfg_.DBData_Host; }
    const std::string get_DBData_User() const { return cfg_.DBData_User; }
    const std::string get_DBData_Pwd() const { return cfg_.DBData_Pwd; }
    const std::string get_DBTask_Host() const { return cfg_.DBTask_Host; }
    const std::string get_DBTask_User() const { return cfg_.DBTask_User; }
    const std::string get_DBTask_Pwd() const { return cfg_.DBTask_Pwd; }
    void output();

    void set_Fid(const std::string& Fid) { cfg_.Fid = Fid; }
    void set_TaskID(const std::string& TaskID) { cfg_.TaskID = TaskID; }
    void set_EngineType(const std::string& EngineType) { cfg_.EngineType = EngineType;} 
    void set_TaskType(const std::string& TaskType) { cfg_.TaskType = TaskType; }
    void set_Task_Shuffle(const int& Task_Shuffle) { cfg_.Task_Shuffle = Task_Shuffle; }
    void set_Thread_Num(const int& Thread_Num) { cfg_.Thread_Num = Thread_Num; }
    void set_Server(const std::string& Server) { cfg_.Server = Server; }
    void set_Ceph_path(const std::string& Ceph_path) { cfg_.Ceph_path = Ceph_path; }
    void set_DBData_Host(const std::string& DBData_Host) { cfg_.DBData_Host = DBData_Host; }
    void set_DBData_User(const std::string& DBData_User) { cfg_.DBData_User = DBData_User; }
    void set_DBData_Pwd(const std::string& DBData_Pwd) { cfg_.DBData_Pwd = DBData_Pwd; }
    void set_DBTask_Host(const std::string& DBTask_Host) { cfg_.DBTask_Host = DBTask_Host; }
    void set_DBTask_User(const std::string& DBTask_User) { cfg_.DBTask_User = DBTask_User; }
    void set_DBTask_Pwd(const std::string& DBTask_Pwd) { cfg_.DBTask_Pwd = DBTask_Pwd; }

   private:
    Config();
    virtual ~Config();
    Config(const Config&);
    Config& operator=(const Config&);
    friend IAocSingleton<Config>;

   private:
    FdConf cfg_;
    std::string selfpath_;
};




#endif //CONFIG_H
