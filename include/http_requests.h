/*================================================================
*   Copyright (C) 2018 . All rights reserved.
*   
*   文件名称：http_requests.h
*   创 建 者：  xuboxuan
*   创建日期：2018年04月13日
*   描    述：
*
================================================================*/


#pragma once

#include <curl/curl.h>
#include "common.h"

namespace seetaas
{
	class Requests_Callback
	{
	public:
		virtual void Progress(void* lParam, double dTotal, double bdowned)=0;
	};

	enum RequestsFlag
	{
		R_None = 0,
		R_Download,
		R_Post,
		R_Get,
	};

	class HttpRequests
	{
	public:
		HttpRequests(void);
		~HttpRequests(void);
		
		bool SetPort(long port);
		bool SetExecutTimeout(int nSecond);//等待返回的超时时间
		bool SetConnectTimeout(int nSecond);//连接的超时时间
		bool SetUserAgent(const char* lpAgent);
		bool SetResumeFrom(long lPos); //断点续传起始位置
		bool AddHeader(string lpKey, string lpValue);
		void ClearHeaderList();//清理头信息
		bool SetCookie(const char* lpCookie);
		//bool SetCookieFile(string lpFilePath);
		const char* GetErrorInfo()const;
		void SetCallback(Requests_Callback* pCallback, void* lParam); //下载进度
		
		bool DownloadFile(const char* lpUrl, const char* lpFile);
		bool Post(const char* lpUrl, const char* lpData);
		bool Post(const char* lpUrl, unsigned char* lpData, unsigned int nSize);//Post字符串或二进制
		
		bool Get(const char* lpUrl);
		
		const string& GetRespons()const;
	protected:

		static size_t WriteCallback(void* pBuffer, size_t nSize, size_t nMemByte, void* pParam)  
		{
		HttpRequests* h_requests = (HttpRequests*)pParam;
		size_t dwriten = 0;
		if(h_requests == nullptr)
			return dwriten;
		switch(h_requests->m_Flag)
		{
		case R_Download:
		    	{
				if(h_requests->fp == nullptr)
					return dwriten;
				if(!fwrite(pBuffer, nSize, nMemByte, h_requests->fp))
					return dwriten;
			}
			break;	
		case R_Post:
		case R_Get:
			{
				h_requests->m_Respons_str.append((const char*)pBuffer, nSize*nMemByte);
				dwriten = nSize*nMemByte;
			}
			break;
		case R_None:
			break;	
		}
		return dwriten;
		}
    		//static size_t   HeaderCallback(void* pBuffer, size_t nSize, size_t nMemByte, void* pParam);  
    		static int ProgressCallback(void *pParam, double dltotal, double dlnow, double ultotal, double ulnow) 
		{
		HttpRequests* h_requests = (HttpRequests*)pParam;
		if(h_requests != nullptr)
		{
			h_requests->m_Callback->Progress(h_requests->m_pCallbackParam, dltotal, dlnow);
		}	
		return 0;
		}	 
	private:
		CURL  *m_pCurl;
		long m_Port;
		FILE* fp;
		CURLcode m_curlCode;
		string m_Respons_str;
		RequestsFlag m_Flag;
		curl_slist *m_headerlist;
		void *m_pCallbackParam;
		Requests_Callback *m_Callback;
	};
}
