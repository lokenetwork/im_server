//
// Created by ubuntu on 1/18/21.
//
#ifndef IM_SERVER_TEXT_H
#define IM_SERVER_TEXT_H
#include "Global.h"
#include "User.h"
class Text {
private:
    Msg::TextMsg txt_msg;
    msg_data* msg_data_p;
    //Msg::TextMsg* txt_msg_p;
    pqxx::connection* Db;

public:
    void send();
    explicit Text(msg_data* _msg_data_p);
    ~Text();
};


#endif //IM_SERVER_TEXT_H
