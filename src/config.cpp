/*================================================================
*   Copyright (C) 2018 . All rights reserved.
*   
*   文件名称：config.cpp
*   创 建 者：  xuboxuan
*   创建日期：2018年04月02日
*   描    述：
*
================================================================*/


#include "include/config.h"

Config::Config() {}

Config::~Config() {}

bool Config::init(const std::string& cfg_path) {
    bool ret = false;
    do {
#ifdef NWITH_FILECONF
        cfg_.Task_Shuffle = "1";
        cfg_.Thread_Num = "1";
        cfg_.DBData_Host = "127.0.0.1";
        cfg_.DBData_User = "root";
        cfg_.DBData_Pwd = "root";
        cfg_.DBTask_Host = "127.0.0.1";
        cfg_.DBTask_User = "root";
        cfg_.DBTask_Pwd = "root";
#else
        std::ifstream f;
        f.open(cfg_path.c_str());
        if (!f.is_open()) {
            printf("Config::init() open config file %s failed",
                cfg_path.c_str());
            break;
        }

	try
	{	
        	Json::Reader r;
        	Json::Value v;
        	if (!r.parse(f, v, false)) {
            		printf("Config::init() parse config %s failed",
                	cfg_path.c_str());
            	break;
        	}

        	cfg_.Task_Shuffle = v[TASK_SHUFFLE].asInt();
        	cfg_.Thread_Num = v[THREAD_NUM].asInt();
        	cfg_.DBData_Host = v[DBDATA_HOST].asString();
        	cfg_.DBData_User = v[DBDATA_USER].asString();
        	cfg_.DBData_Pwd = v[DBDATA_PWD].asString();
        	cfg_.DBTask_Host = v[DBTASK_HOST].asString();
        	cfg_.DBTask_User = v[DBTASK_USER].asString();
        	cfg_.DBTask_Pwd = v[DBTASK_PWD].asString();
	}
	catch(...)
	{
		LEANO_LOG_ERROR << "Parse json config" << cfg_path << "wrong";
		return ret;	
	}
        
#endif
        cfg_.Fid = "";
        cfg_.TaskID = "";
        cfg_.EngineType = "unknown";
        cfg_.TaskType = "unknown";
        cfg_.Server = "127.0.0.1";
        cfg_.Ceph_path = "";
        ret = true;
    } while (0);
    
    return ret;
}

void Config::output() {
        LEANO_LOG_INFO << "Fid: " << cfg_.Fid;
        LEANO_LOG_INFO << "TaskID: " << cfg_.TaskID;
        LEANO_LOG_INFO << "EngineType: "<<  cfg_.EngineType;
        LEANO_LOG_INFO << "TaskType: " << cfg_.TaskType;
        LEANO_LOG_INFO << "Task_Shuffle: " << cfg_.Task_Shuffle;
        LEANO_LOG_INFO << "Thread_Num: " << cfg_.Thread_Num;
        LEANO_LOG_INFO << "Server: " << cfg_.Server;
        LEANO_LOG_INFO << "Ceph_path: " << cfg_.Ceph_path;
        LEANO_LOG_INFO << "DBData_Host: " << cfg_.DBData_Host;
        LEANO_LOG_INFO << "DBData_User: " << cfg_.DBData_User;
        LEANO_LOG_INFO << "DBData_Pwd: " << cfg_.DBData_Pwd;
        LEANO_LOG_INFO << "DBTask_Host: " << cfg_.DBTask_Host;
        LEANO_LOG_INFO << "DBTask_User: " << cfg_.DBTask_User;
        LEANO_LOG_INFO << "DBTask_Pwd: " << cfg_.DBTask_Pwd;
}
