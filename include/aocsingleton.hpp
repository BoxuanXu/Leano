/*================================================================
*   Copyright (C) 2018 . All rights reserved.
*   
*   文件名称：aocsingleton.hpp
*   创 建 者：  xuboxuan
*   创建日期：2018年04月02日
*   描    述：
*
================================================================*/


#ifndef _AOCSINGLETON_H
#define _AOCSINGLETON_H


template <class T>
class IAocSingleton {
   private:
    IAocSingleton(const IAocSingleton&);
    IAocSingleton& operator=(const IAocSingleton&);

   public:
    static T* get_instance() {
        if (!instance_) instance_ = new T;
        return instance_;
    }

    static void release() {
        if (instance_) {
            delete instance_;
            instance_ = 0;
        }
    }

   protected:
    IAocSingleton(){};
    ~IAocSingleton(){};
    static T* instance_;
};

template <class T>
T* IAocSingleton<T>::instance_ = 0;


#endif //AOCSINGLETON_H
