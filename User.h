//
// Created by ubuntu on 1/18/21.
//
#ifndef IM_SERVER_USER_H
#define IM_SERVER_USER_H
#include "Global.h"
class User {

private:


public:
    int get_user_id_from_fd(UDTSOCKET fd);
    explicit User();
    ~User();

};


#endif //IM_SERVER_USER_H
