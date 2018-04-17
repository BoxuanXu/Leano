/*================================================================
*   Copyright (C) 2018 . All rights reserved.
*   
*   文件名称：DBOperation.cpp
*   创 建 者：  xuboxuan
*   创建日期：2018年04月02日
*   描    述：
*
================================================================*/


#include "include/DBOperation.h"


DBOperation::DBOperation(string url, string user, string pwd)
{
	try
	{
		mysql::MySQL_Driver *driver = mysql::get_driver_instance();
		if(driver == NULL)
		{
			LEANO_LOG_ERROR << "get driver error";
			useable = false;
		}
		conn = driver->connect(url, user, pwd);
		//state = conn->createStatement();
		if(!conn)
		{
			useable = false;
			return;
		}
		LEANO_LOG_INFO << "connect to database OK";
		useable = true;
	}
	catch(...)
	{
		LEANO_LOG_ERROR << "mysql connect wrong";
		useable = false;
	}
}


DBOperation::~DBOperation()
{
	useable = false;
	if(res)  
	{  
       		delete res;  
		res = NULL;  
    	}  
	if(state)  
    	{  
        	delete state;  
        	state = NULL;  
    	}  
    	if(conn)  
    	{  
        	conn->close();  
        	delete conn;  
        	conn = NULL;  
    	}	  
}

bool DBOperation::SetDatabase(const string& tablename)
{
	try
	{
		conn->setSchema(tablename);
		state = conn->createStatement();
		return true;
	}
	catch(...)
	{
		LEANO_LOG_ERROR << "set database wrong " << tablename;
		return false;
	}
}

void DBOperation::Execute(const SQLString& sql)
{
	if(!state)  
	{  
        	state = conn->createStatement();  
    	}  
    	state->execute(sql);  
}



bool DBOperation::ExecuteQuery(const SQLString& sql)
{
	try
	{
		if (!state)  
    		{  
        		state = conn->createStatement();  
    		}  
    		res = state->executeQuery(sql);  
    	
    		if(res)  
        		return true;  
    		else  
        		return false;
	}
	catch(...)
	{
		LEANO_LOG_ERROR << "execute query wrong, sql:" << sql;
		return false;
	}  
}

bool DBOperation::ExecuteUpdate(const SQLString& sql)
{
	//savept = conn->setSavepoint("savept");
	sql::PreparedStatement *prep_stmt = NULL;		  
    	try{  
        	//conn->setTransactionIsolation(TRANSACTION_SERIALIZABLE); //when change the data,we can't read the data  
        	prep_stmt = conn->prepareStatement(sql);  
        	int updatecount = prep_stmt->executeUpdate();  
        	if(updatecount == 0)  
        	{  
            		printf("no rows update\n");  
            		//return false;  
        	} 
        	conn->commit();
		if(prep_stmt)  
    		{  
        		delete prep_stmt;  
        		prep_stmt = NULL;  
    		}  	
		return true; 
    	}catch(SQLException &e)  
    	{  
        	cout << "ERROR: SQLException in " << __FILE__;  
        	cout << " (" << __func__<< ") on line " << __LINE__ << endl;  
        	cout << "ERROR: " << e.what();  
        	cout << " (MySQL error code: " << e.getErrorCode();  
        	cout << ", SQLState: " << e.getSQLState() << ")" << endl;  
        	//RollBack(savept);  
		if(prep_stmt)  
    		{  
        		delete prep_stmt;  
        		prep_stmt = NULL;  
    		}  	
            	return false;  
    	}     
}
void DBOperation::RollBack(Savepoint* savepoint)
{
	conn->rollback(savepoint);  
    	conn->releaseSavepoint(savepoint); 
}

int DBOperation::getColumnCount()
{
   try
   {
     	if(res == NULL)  
    	{  
        	return -1;  
    	}  
    	if(rs_meta == NULL)  
    	{  
	    	rs_meta = res->getMetaData();  
    	}  
    	int cols = rs_meta->getColumnCount();  
    	return cols; 
    }
    catch(...)
    {
	LEANO_LOG_ERROR << "get Column Count wrong";
	return -1;	
    } 
}

int DBOperation::getRowCount()
{
	int rows = 0;  
    	rows = res->rowsCount();  
    	return rows;  
}

string DBOperation::getColumnName(int index)
{
    	if(rs_meta == NULL)  
    	{  
        	rs_meta = res->getMetaData();  
    	}  
    	string columnname = rs_meta->getColumnLabel(index+1);  
    	return columnname;  
}


int DBOperation::getValueInt(const string& columnname)
{
	int value = res->getInt(columnname);  
    	return value;  
}

string DBOperation::getValueString(const string& columnname)
{
	string value;
	try
	{
    		value = res->getString(columnname);  
	}
	catch(...)
	{
		LEANO_LOG_ERROR << "get value string wrong , string:" << columnname;
		value = "";	
	}  
    	return value;
}  

bool DBOperation::getNext()
{
	return res->next();
} 		
