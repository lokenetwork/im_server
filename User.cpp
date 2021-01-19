//
// Created by ubuntu on 1/18/21.
//
#include "User.h"
//TODO,User 类封装成静态方法，研究一下 C++ 的静态方法会不会调析构函数。
User::User() {

}
User::~User() {

}

int User::get_user_id_from_fd(UDTSOCKET fd) {
    map<UDTSOCKET ,int32_t>::iterator fd_iter = Global::fd_map.find(fd);
    return fd_iter->second;
}