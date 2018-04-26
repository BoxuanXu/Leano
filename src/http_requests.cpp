/*================================================================
*   Copyright (C) 2018 . All rights reserved.
*
*   文件名称：http_requests.cpp
*   创 建 者：  xuboxuan
*   创建日期：2018年04月13日
*   描    述：
*
================================================================*/

#include "include/http_requests.h"

namespace seetaas {
HttpRequests::HttpRequests(void)
    : m_pCurl(nullptr),
      m_Port(8080),
      fp(nullptr),
      m_curlCode(CURLE_OK),
      m_Flag(R_None),
      m_headerlist(nullptr),
      m_pCallbackParam(nullptr),
      m_Callback(nullptr) {
  curl_global_init(CURL_GLOBAL_ALL);
  m_pCurl = curl_easy_init();
  curl_easy_setopt(m_pCurl, CURLOPT_WRITEFUNCTION, WriteCallback);
  curl_easy_setopt(m_pCurl, CURLOPT_WRITEDATA, this);
}

HttpRequests::~HttpRequests(void) {
  ClearHeaderList();
  curl_easy_cleanup(m_pCurl);
  curl_global_cleanup();
}

bool HttpRequests::SetPort(long port) {
  if (port == m_Port) return true;
  m_Port = port;
  m_curlCode = curl_easy_setopt(m_pCurl, CURLOPT_PORT, m_Port);
  return CURLE_OK == m_curlCode;
}

bool HttpRequests::SetExecutTimeout(int nSecond) {
  if (nSecond < 0) {
    LEANO_LOG_ERROR << "SetExecuteTimeout wrong";
    return false;
  }
  m_curlCode = curl_easy_setopt(m_pCurl, CURLOPT_TIMEOUT, nSecond);
  return CURLE_OK == m_curlCode;
}

bool HttpRequests::SetConnectTimeout(int nSecond) {
  if (nSecond < 0) {
    LEANO_LOG_ERROR << "SetConnectTimeout wrong";
    return false;
  }
  m_curlCode = curl_easy_setopt(m_pCurl, CURLOPT_CONNECTTIMEOUT, nSecond);
  return CURLE_OK == m_curlCode;
}

bool HttpRequests::SetUserAgent(const char* lpAgent) {
  if (lpAgent == nullptr) {
    LEANO_LOG_ERROR << "SetUserAgent wrong";
    return false;
  }

  m_curlCode = curl_easy_setopt(m_pCurl, CURLOPT_USERAGENT, lpAgent);
  return CURLE_OK == m_curlCode;
}

bool HttpRequests::SetResumeFrom(long lPos) {
  if (lPos < 0) return false;

  m_curlCode = curl_easy_setopt(m_pCurl, CURLOPT_RESUME_FROM, lPos);
  return CURLE_OK == m_curlCode;
}

bool HttpRequests::AddHeader(string lpKey, string lpValue) {
  if (lpKey.length() == 0 || lpValue.length() == 0) {
    LEANO_LOG_ERROR << "AddHeader wrong";
    return false;
  }
  lpKey.append(": ");
  lpKey.append(lpValue);
  m_headerlist = curl_slist_append(m_headerlist, lpKey.c_str());

  m_curlCode = curl_easy_setopt(m_pCurl, CURLOPT_HTTPHEADER, m_headerlist);
  return CURLE_OK == m_curlCode;
}

void HttpRequests::ClearHeaderList() {
  if (m_headerlist) {
    curl_slist_free_all(m_headerlist);
    m_headerlist = nullptr;
  }
}

bool HttpRequests::SetCookie(const char* lpCookie) {
  if (lpCookie == nullptr) {
    LEANO_LOG_ERROR << "Set Cookie wrong";
    return false;
  }
  m_curlCode = curl_easy_setopt(m_pCurl, CURLOPT_COOKIE, lpCookie);
  return CURLE_OK == m_curlCode;
}

/*bool HttpRequests::SetCookieFile(string lpFilePath)
{
        if(lpFilePath.length() == 0)
                return false;

        m_curlCode = curl_easy_setopt(m_pCurl, CURLOPT_COOKIE, lpFilePath);
        return CURLE_OK == m_curlCode;

}*/

const char* HttpRequests::GetErrorInfo() const {
  return curl_easy_strerror(m_curlCode);
}

void HttpRequests::SetCallback(Requests_Callback* pCallback, void* lParam) {
  m_pCallbackParam = lParam;
  m_Callback = pCallback;
}

bool HttpRequests::DownloadFile(const char* lpUrl, const char* lpFile) {
  if (lpUrl == nullptr || lpFile == nullptr) {
    LEANO_LOG_ERROR << "DownloadFile wrong";
    return false;
  }
  curl_easy_setopt(m_pCurl, CURLOPT_URL, lpUrl);

  fp = ::fopen(lpUrl, "ab+");
  if (fp == nullptr) {
    LEANO_LOG_ERROR << "DownloadFile wrong";
    return false;
  }

  curl_easy_setopt(m_pCurl, CURLOPT_NOPROGRESS, 0);
  curl_easy_setopt(m_pCurl, CURLOPT_PROGRESSFUNCTION, ProgressCallback);
  curl_easy_setopt(m_pCurl, CURLOPT_PROGRESSDATA, this);
  m_Flag = R_Download;
  m_curlCode = curl_easy_perform(m_pCurl);
  return CURLE_OK == m_curlCode;
}

bool HttpRequests::Post(const char* lpUrl, const char* lpData) {
  if (lpUrl == nullptr || lpData == nullptr || m_pCurl == nullptr) {
    LEANO_LOG_ERROR << "Post wrong";
    return false;
  }
  curl_easy_setopt(m_pCurl, CURLOPT_POST, 1);
  curl_easy_setopt(m_pCurl, CURLOPT_URL, lpUrl);
  curl_easy_setopt(m_pCurl, CURLOPT_POSTFIELDS, lpData);

  m_Flag = R_Post;
  m_Respons_str.clear();
  m_curlCode = curl_easy_perform(m_pCurl);
  return CURLE_OK == m_curlCode;
}

bool HttpRequests::Post(const char* lpUrl, unsigned char* lpData,
                        unsigned int nSize) {
  if (lpUrl == nullptr || lpData == nullptr || nSize < 0 ||
      m_pCurl == nullptr) {
    LEANO_LOG_ERROR << "Post wrong";
    return false;
  }
  curl_easy_setopt(m_pCurl, CURLOPT_POST, 1);
  curl_easy_setopt(m_pCurl, CURLOPT_POSTFIELDS, lpData);
  curl_easy_setopt(m_pCurl, CURLOPT_POSTFIELDSIZE, nSize);
  curl_easy_setopt(m_pCurl, CURLOPT_URL, lpUrl);

  m_Flag = R_Post;
  m_Respons_str.clear();
  m_curlCode = curl_easy_perform(m_pCurl);
  return CURLE_OK == m_curlCode;
}

bool HttpRequests::Get(const char* lpUrl) {
  if (lpUrl == nullptr) {
    LEANO_LOG_ERROR << "Get wrong";
    return false;
  }

  curl_easy_setopt(m_pCurl, CURLOPT_HTTPGET, 1);
  curl_easy_setopt(m_pCurl, CURLOPT_URL, lpUrl);
  curl_easy_setopt(m_pCurl, CURLOPT_FOLLOWLOCATION, 1);
  curl_easy_setopt(m_pCurl, CURLOPT_SSL_VERIFYPEER, 0);
  curl_easy_setopt(m_pCurl, CURLOPT_SSL_VERIFYHOST, 0);

  m_Flag = R_Get;
  m_Respons_str.clear();
  m_curlCode = curl_easy_perform(m_pCurl);
  return CURLE_OK == m_curlCode;
}

const string& HttpRequests::GetRespons() const { return m_Respons_str; }

/*size_t WriteCallback(void* pBuffer, size_t nSize, size_t nMemByte, void*
pParam)
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
                        h_requests->m_Respons_str.append((const char*)pBuffer,
nSize*nMemByte);
                        dwriten = nSize*nMemByte;
                }
                break;
        case R_None:
                break;
        }
        return dwriten;
}*/

/*int ProgressCallback(void *pParam, double dltotal, double dlnow, double
ultotal, double ulnow)
{
        HttpRequests* h_requests = (HttpRequests*)pParam;
        if(h_requests != nullptr)
        {
                h_requests->m_Callback->Progress(h_requests->m_pCallbackParam,
dltotal, dlnow);
        }
        return 0;
}*/
}
