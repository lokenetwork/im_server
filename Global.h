//
// Created by ubuntu on 1/11/21.
//
#ifndef IM_TEST_GLOBAL_H
#define IM_TEST_GLOBAL_H
#define MAXLINE 1024
#define NUMTHREAD 2
#define LISTEN_BACKLOG 100
#include <pqxx/pqxx>
#include <mutex>
#include <deque>
#include <iostream>
#include <iostream>
#include <udt.h>
#include <mutex>
#include <memory>
#include <map>
#include <condition_variable>
#include "msg.pb.h"
using namespace std;
typedef pair<shared_ptr<char>,UDTSOCKET> msg_data;
namespace Global {
    extern mutex text_mutex;
    extern condition_variable text_cv;
    extern deque<msg_data> msg_data_deq;
    extern mutex client_list_mutex;
    extern map<int32_t ,set<UDTSOCKET>> client_list;
    //要关联fd跟 user_id 的关系。
    extern map<UDTSOCKET ,int32_t> fd_map;
}

#endif //IM_TEST_GLOBAL_H
