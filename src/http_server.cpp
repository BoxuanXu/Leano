/*================================================================
*   Copyright (C) 2018 . All rights reserved.
*   
*   文件名称：http_server.cpp
*   创 建 者：  xuboxuan
*   创建日期：2018年04月12日
*   描    述：
*
================================================================*/


#include "include/http_server.h"

namespace seetaas{

unordered_map<string, ReqHandler> HttpServer::s_handler_map;
crow::App<Middleware> HttpServer::app;

void HttpServer::Init(const string & port,const string & Message)
{
	m_port = port;
	app.get_middleware<Middleware>().setMessage(Message);
}

bool HttpServer::Start()
{
	uint16_t port = 0;
	try{
		port = boost::lexical_cast<uint16_t>(m_port);		
	}
	catch(boost::bad_lexical_cast& e)
	{
		return false;
	}
	app.port(port)
	   .multithreaded()
	   .run();
}

void HttpServer::AddPostHandler(string &url,  ReqHandler req_handler)
{
	if(s_handler_map.find(url) != s_handler_map.end())
		return;	

	app.route_dynamic(move(url)).methods("POST"_method)
	(req_handler);

	s_handler_map.insert(make_pair(url, req_handler));
}

void HttpServer::AddGetHandler(string &url,  ReqHandler req_handler)
{
	if(s_handler_map.find(url) != s_handler_map.end())
		return;	

	constexpr crow::black_magic::const_str url_str("/");
	
	app.route_dynamic(move(url))
	(req_handler);

	s_handler_map.insert(make_pair(url, req_handler));
}

}
