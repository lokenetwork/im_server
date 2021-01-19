//
// Created by ubuntu on 1/9/21.
//
#include "Login.h"
#include "Contact.h"
Login::Login(msg_data* _msg_data_p) {
    login_msg.ParseFromArray(_msg_data_p->first.get(),MAXLINE);
    //TODO，后面用连接池优化。
    //获取数据库连接。
    Db = new pqxx::connection("dbname=im user=ubuntu password=ubuntu hostaddr=192.168.0.113 port=5432");
    Db->set_client_encoding("UTF8");
    this->name = login_msg.name();
    this->password = login_msg.password();
    this->from_fd = _msg_data_p->second;
}
Login::~Login() {
    //TODO，用智能指针优化。
    //断开数据库连接。
    Db->disconnect();
    delete Db;
}

int Login::go() {
    user_id = this->get_user_id();
    if (user_id > 0) {
        cout << "new login " << name << "-" << password << "，user_id:" << user_id << ",fd:"
             << from_fd << endl;
        this->success();
    } else {
        this->fail();
    }
}

int Login::get_user_id() {
    /*
     * 查询数据库账号密码，大于0正确，0，账号密码错误。
     * */
    pqxx::nontransaction ntx(*Db);
    string sql =
            "SELECT * FROM users WHERE name = '" + ntx.esc(name) + "'" + " AND password = '" + ntx.esc(password) + "'";
    pqxx::result r(ntx.exec(sql));
    int const num_rows = r.size();
    if (num_rows != 0) {
        return r[0][0].as<int>();
    } else {
        return 0;
    }
}

int Login::success() {
    /* 1，操作client_list 需要加锁
     * 2，判断map是否已经存在，不存在重建，存在插入
     * 3，绑定fd对应的user_id
     */
    std::unique_lock <std::mutex> client_list_lock(Global::client_list_mutex);
    map < int32_t, std::set < UDTSOCKET >> ::iterator
    client_info = Global::client_list.find(user_id);
    if (client_info == Global::client_list.end()) {
        set <UDTSOCKET> user_fds;
        user_fds.insert(from_fd);
        Global::client_list.insert(map < int32_t, std::set < UDTSOCKET >> ::value_type(user_id, user_fds));
    } else {
        client_info->second.insert(from_fd);
    }
    client_list_lock.unlock();
    Global::fd_map.insert(map<UDTSOCKET, int32_t>::value_type(from_fd, user_id));

    //发送登录成功。
    Msg::LoginMsgRes login_msg_res;
    login_msg_res.set_msg_type(Msg::TYPE_LOGIN_R);
    login_msg_res.set_code(Msg::CODE_SUCCESS);
    login_msg_res.set_user_id(user_id);
    size_t size = login_msg_res.ByteSizeLong();
    shared_ptr<char> buffer(new char[size]());
    login_msg_res.SerializeToArray(buffer.get(), size);
    if (UDT::ERROR == UDT::sendmsg(from_fd, buffer.get(), size)) {
        cout << "send error: " << UDT::getlasterror().getErrorMessage();
    }
    //TODO，写入登录日志。

    //发送联系人列表。
    Contact contact(user_id,from_fd);
    contact.send_list();
}

int Login::fail() {
    //返回登录失败。
    Msg::LoginMsgRes login_msg_res;
    login_msg_res.set_msg_type(Msg::TYPE_LOGIN_R);
    login_msg_res.set_code(Msg::CODE_PASSWORD_WRONG);
    login_msg_res.set_user_id(0);
    size_t size = login_msg_res.ByteSizeLong();
    shared_ptr<char> buffer(new char[size]());
    login_msg_res.SerializeToArray(buffer.get(), size);
    if (UDT::ERROR == UDT::sendmsg(from_fd, buffer.get(), size)) {
        cout << "send error: " << UDT::getlasterror().getErrorMessage();
    }
    return 0;
}

