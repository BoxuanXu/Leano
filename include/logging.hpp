/*================================================================
*   Copyright (C) 2018 . All rights reserved.
*   
*   文件名称：te.h
*   创 建 者：  xuboxuan
*   创建日期：2018年03月29日
*   描    述：
*
================================================================*/
#pragma once
#include <string>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <sstream>

#define CROW_ENABLE_LOGGING

#ifndef CROW_LOG_LEVEL
#define CROW_LOG_LEVEL 1
#endif


namespace seetaas 
{
    enum class LogLevel
    {
/*#ifndef ERROR
        DEBUG = 0,
        INFO,
        WARNING,
        ERROR,
        CRITICAL,
#endif*/

        Debug = 0,
        Info,
        Warning,
        Error,
        Critical,
    };

    class ILogHandler {
        public:
            virtual void log(std::string message, LogLevel level) = 0;
    };

    class CerrLogHandler : public ILogHandler {
        public:
            void log(std::string message, LogLevel /*level*/) override {
                std::cerr << message;
            }
    };

    class logger {

        private:
            //
            static std::string timestamp()
            {
                char date[32];
                time_t t = time(0);

                tm my_tm;

#if defined(_MSC_VER) or defined(__MINGW32__)
                gmtime_s(&my_tm, &t);
#else
                gmtime_r(&t, &my_tm);
#endif

                size_t sz = strftime(date, sizeof(date), "%Y-%m-%d %H:%M:%S", &my_tm);
                return std::string(date, date+sz);
            }

        public:


            logger(std::string prefix, LogLevel level) : level_(level) {
    #ifdef CROW_ENABLE_LOGGING
                    stringstream_ << "(" << timestamp() << ") [" << prefix << "] ";
    #endif

            }
            ~logger() {
    #ifdef CROW_ENABLE_LOGGING
                if(level_ >= get_current_log_level()) {
                    stringstream_ << std::endl;
                    get_handler_ref()->log(stringstream_.str(), level_);
                }
    #endif
            }

            //
            template <typename T>
            logger& operator<<(T const &value) {

    #ifdef CROW_ENABLE_LOGGING
                if(level_ >= get_current_log_level()) {
                    stringstream_ << value;
                }
    #endif
                return *this;
            }

            //
            static void setLogLevel(LogLevel level) {
                get_log_level_ref() = level;
            }

            static void setHandler(ILogHandler* handler) {
                get_handler_ref() = handler;
            }

            static LogLevel get_current_log_level() {
                return get_log_level_ref();
            }

        private:
            //
            static LogLevel& get_log_level_ref()
            {
                static LogLevel current_level = (LogLevel)CROW_LOG_LEVEL;
                return current_level;
            }
            static ILogHandler*& get_handler_ref()
            {
                static CerrLogHandler default_handler;
                static ILogHandler* current_handler = &default_handler;
                return current_handler;
            }

            //
            std::ostringstream stringstream_;
            LogLevel level_;
    };
}

#define LEANO_LOG_CRITICAL   \
        if (seetaas::logger::get_current_log_level() <= seetaas::LogLevel::Critical) \
            seetaas::logger("CRITICAL", seetaas::LogLevel::Critical)
#define LEANO_LOG_ERROR      \
        if (seetaas::logger::get_current_log_level() <= seetaas::LogLevel::Error) \
            seetaas::logger("ERROR   ", seetaas::LogLevel::Error)
#define LEANO_LOG_WARNING    \
        if (seetaas::logger::get_current_log_level() <= seetaas::LogLevel::Warning) \
            seetaas::logger("WARNING ", seetaas::LogLevel::Warning)
#define LEANO_LOG_INFO       \
        if (seetaas::logger::get_current_log_level() <= seetaas::LogLevel::Info) \
            seetaas::logger("INFO    ", seetaas::LogLevel::Info)
#define LEANO_LOG_DEBUG      \
        if (seetaas::logger::get_current_log_level() <= seetaas::LogLevel::Debug) \
            seetaas::logger("DEBUG   ", seetaas::LogLevel::Debug)

