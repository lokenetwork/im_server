//
// Created by ubuntu on 1/18/21.
//
#include "Text.h"
Text::Text(msg_data* _msg_data_p) {
    //初始化数据
    msg_data_p = _msg_data_p;
    txt_msg.ParseFromArray(msg_data_p->first.get(),MAXLINE);
    User user;
    txt_msg.set_from_user(user.get_user_id_from_fd(msg_data_p->second));
    cout << "from user_id " << txt_msg.from_user() << " to " << txt_msg.to_user() << ",text_msg:" << txt_msg.data() << endl;

    Db = new pqxx::connection("dbname=im user=ubuntu password=ubuntu hostaddr=192.168.0.113 port=5432");
    Db->set_client_encoding("UTF8");
}

Text::~Text() {
    //TODO，用智能指针优化。
    //断开数据库连接。
    Db->disconnect();
    delete Db;
}

void Text::send() {
    //查找对应客户端fd，遍历发送。
    map<int32_t ,std::set<UDTSOCKET>>::iterator client_iter = Global::client_list.find(txt_msg.to_user());
    size_t size = txt_msg.ByteSizeLong();
    shared_ptr<char> buffer(new char[size]());
    txt_msg.SerializeToArray(buffer.get(), size);
    bool is_read;
    if(client_iter != Global::client_list.end()){
        is_read = true;
        for(std::set<UDTSOCKET>::iterator iter = client_iter->second.begin(); iter!=client_iter->second.end(); iter++)
        {
            cout << "send to fd " << *iter << endl ;
            if (UDT::ERROR == UDT::sendmsg(*iter,buffer.get(),size))
            {
                cout << "send error: " << UDT::getlasterror().getErrorMessage();
            }
        }
    }else{
        is_read = false;
    }
    //store database
    pqxx::nontransaction ntx(*Db);
    string sql =
            "INSERT INTO message(\"from_uid\", \"to_uid\", \"type\", \"data\") "
            "VALUES ( "+to_string(txt_msg.from_user())+", "+to_string(txt_msg.to_user())+", "+to_string(Msg::TYPE_TEXT)+", '"+ntx.esc(txt_msg.data())+"')";
    ntx.exec(sql);
}