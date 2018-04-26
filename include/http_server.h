/*================================================================
*   Copyright (C) 2018 . All rights reserved.
*
*   文件名称：http_server.h
*   创 建 者：  xuboxuan
*   创建日期：2018年04月12日
*   描    述：
*
================================================================*/

#pragma once

#include <boost/lexical_cast.hpp>
#include <functional>
#include "common.h"
#include "crow.h"
namespace seetaas {
struct Middleware {
  std::string message;

  Middleware() { message = "foo"; }

  void setMessage(std::string newMsg) { message = newMsg; }

  struct context {};

  void before_handle(crow::request& /*req*/, crow::response& /*res*/,
                     context& /*ctx*/) {
    // LEANO_LOG_INFO  << " - MESSAGE: " << message;
  }

  void after_handle(crow::request& /*req*/, crow::response& /*res*/,
                    context& /*ctx*/) {
    // no-op
  }
};

using ReqHandler = std::function<void(const crow::request&, crow::response&)>;
// using ReqHandler = std::function<bool(const crow::request&)>;

typedef struct crow_res_req {
  crow::request req;
  crow::response res;
} CROW_RES_REQ;

class HttpServer {
 public:
  HttpServer() {}
  ~HttpServer() {}
  void Init(const string& port, const string& Message);
  bool Start();
  void AddPostHandler(string& url,
                      ReqHandler req_handler);  //注册Post类型URL回调函数
  void AddGetHandler(string& url,
                     ReqHandler req_handler);  //注册Get类型URL回调函数
  static unordered_map<string, ReqHandler> s_handler_map;  //回调函数映射表

 private:
  string m_port;
  static crow::App<Middleware> app;
};
}
