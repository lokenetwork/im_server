//
// Created by ubuntu on 1/13/21.
//
#ifndef IM_SERVER_CONTACT_H
#define IM_SERVER_CONTACT_H
#include "Global.h"
class Contact {

private:
    UDTSOCKET from_fd;
    pqxx::connection* Db;
    int user_id;

public:
    //发送用户列表
    void send_list();
    explicit Contact(int _user_id,UDTSOCKET _from_fd);
    ~Contact();
};
#endif //IM_SERVER_CONTACT_H
