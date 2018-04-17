/*================================================================
*   Copyright (C) 2018 . All rights reserved.
*   
*   文件名称：DBOperation.h
*   创 建 者：  xuboxuan
*   创建日期：2018年04月02日
*   描    述：
*
================================================================*/


#pragma once

#include "connection_pool.h"

class DBOperation
{
public:
	explicit DBOperation(string url = "tcp://127.0.0.1/3306", string user="root", string pwd="root");
	virtual ~DBOperation();

	void Execute(const SQLString& sql);
	
	bool ExecuteQuery(const SQLString& sql);
	
	bool ExecuteUpdate(const SQLString& sql);
	
	void RollBack(Savepoint* savepoint = NULL);

	int getColumnCount();
	
	int getRowCount();

	string getColumnName(int index);

	bool SetDatabase(const string& tablename);	
	
	int getValueInt(const string& columnname);  
    
	string getValueString(const string& columnname);  
    
	bool getNext();  	
	
	bool useable = false;	
private:
	Connection *conn=NULL;
	Statement *state=NULL;  
    	ResultSet *res=NULL;  
	Savepoint *savept=NULL; 
	ResultSetMetaData *rs_meta=NULL;	
};
