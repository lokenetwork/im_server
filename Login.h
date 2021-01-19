//
// Created by ubuntu on 1/9/21.
//
#ifndef IM_SERVER_LOGIN_H
#define IM_SERVER_LOGIN_H
#include "Global.h"
class Login {
private:
    Msg::LoginMsgReq login_msg;
    string name;
    string password;
    int user_id;
    UDTSOCKET from_fd;
    bool login_success = false;
    int insert_log(); //处理登录日志。
    pqxx::connection* Db;
public:
    int go();
    int get_user_id();
    int success();
    int fail();
    explicit Login(msg_data* _msg_data_p);
    ~Login();
};
#endif //IM_SERVER_LOGIN_H
