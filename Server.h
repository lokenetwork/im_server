//
// Created by ubuntu on 1/19/21.
//
#ifndef IM_SERVER_SERVER_H
#define IM_SERVER_SERVER_H
#include <arpa/inet.h>
#include "Global.h"
class Server {
private:
    //服务端监听套接字。
    UDTSOCKET serv;
    int epoll_id;
    std::set<UDTSOCKET> ready_fd;
    int watch_events = UDT_EPOLL_IN;
public:
    int listen();
    void wait();
    int accept(UDTSOCKET read_fd);
    //fd关闭的清理工作。
    void close_fd(UDTSOCKET fd);
    explicit Server();
    ~Server();
};
#endif //IM_SERVER_SERVER_H
