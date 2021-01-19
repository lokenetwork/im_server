//
// Created by ubuntu on 1/13/21.
//
#include "Contact.h"

Contact::Contact(int _user_id, UDTSOCKET _from_fd) {
    //TODO，后面用连接池优化。
    //获取数据库连接。
    Db = new pqxx::connection("dbname=im user=ubuntu password=ubuntu hostaddr=192.168.0.113 port=5432");
    Db->set_client_encoding("UTF8");
    this->user_id = _user_id;
    this->from_fd = _from_fd;
}
Contact::~Contact() {
    //TODO，用智能指针优化。
    //断开数据库连接。
    Db->disconnect();
    delete Db;
}

void Contact::send_list()  {
    /*
    * 查询用户列表,发送
    * TODO，后面分页。
    */
    pqxx::nontransaction ntx(*Db);
    string sql = "SELECT users_contact.friend_id,users.name,users_contact.last_msg FROM users_contact LEFT JOIN users ON users.id = users_contact.friend_id WHERE users_contact.user_id = "+ntx.esc(to_string(this->user_id))+" LIMIT 100;";
    pqxx::result r(ntx.exec(sql));

    int const num_rows = r.size();
    if( num_rows > 0 ){
        Msg::ContactMsgRes contactMsgRes;
        contactMsgRes.set_msg_type(Msg::TYPE_CONTACT_R);
        for (int rownum=0; rownum < num_rows; ++rownum)
        {
            auto const row = r[rownum];
            Msg::User* user;
            user = contactMsgRes.add_user();
            user->set_id(row[0].as<int>());
            user->set_name(row[1].c_str());
            user->set_last_msg(row[2].c_str());
        }
        size_t size = contactMsgRes.ByteSizeLong();
        //TODO，这里没必要用智能指针。可以用栈内存直接搞定。
        shared_ptr<char> buffer(new char[size]());
        contactMsgRes.SerializeToArray(buffer.get(), size);
        if (UDT::ERROR == UDT::sendmsg(from_fd, buffer.get(), size)) {
            cout << "send error: " << UDT::getlasterror().getErrorMessage();
        }
    }else{
        //TODO，发送无联系人的消息
    }


}