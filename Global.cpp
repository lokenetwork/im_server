//
// Created by ubuntu on 1/11/21.
//
#include "Global.h"
mutex Global::text_mutex;
condition_variable Global::text_cv;
deque<msg_data> Global::msg_data_deq;
mutex Global::client_list_mutex;
map<int32_t ,set<UDTSOCKET>> Global::client_list;
//要关联fd跟 user_id 的关系。
map<UDTSOCKET ,int32_t> Global::fd_map;