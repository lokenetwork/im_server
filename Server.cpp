//
// Created by ubuntu on 1/19/21.
//
//服务器主函数。
#include "Server.h"
Server::Server() {
}
Server::~Server() {
}
//服务端监听初始化。
int Server::listen() {
    UDT::startup();
    serv = UDT::socket(AF_INET, SOCK_DGRAM, 0);
    sockaddr_in my_addr;
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(9000);
    my_addr.sin_addr.s_addr = INADDR_ANY;
    memset(&(my_addr.sin_zero), '\0', 8);
    if (UDT::ERROR == UDT::bind(serv, (sockaddr * ) & my_addr, sizeof(my_addr))) {
        return 1;
    }
    UDT::listen(serv, LISTEN_BACKLOG);
    epoll_id = UDT::epoll_create();
    if( 0 != UDT::epoll_add_usock(epoll_id, serv, &watch_events) ){
        return 1;
    }else{
        return 0;
    }
}

int Server::accept(UDTSOCKET read_fd) {
    //处理新连接。
    int namelen;
    sockaddr_in their_addr;
    UDTSOCKET client = UDT::accept(read_fd, (sockaddr * ) & their_addr, &namelen);
    cout << "new connection: " << inet_ntoa(their_addr.sin_addr) << ":" << ntohs(their_addr.sin_port) << ",fd:" << client << endl;
    //把 fd 加进去epoll
    if( 0 != UDT::epoll_add_usock( epoll_id, client, &watch_events) ){
        cout << "add client socket watch fail:" << endl;
        return 1;
    }else{
        return 0;
    }
}

void Server::close_fd(UDTSOCKET fd) {
    /*
     * 删除 epoll 中的fd，跟fd_map，client_list 中的相关数据
     * */
    UDT::epoll_remove_usock(epoll_id, fd);
    map<UDTSOCKET ,int32_t>::iterator fd_iter = Global::fd_map.find(fd);
    if(fd_iter != Global::fd_map.end()){
        map<int32_t ,std::set<UDTSOCKET>>::iterator client_iter = Global::client_list.find(fd_iter->second);
        if(client_iter != Global::client_list.end()){
            std::set<UDTSOCKET>::iterator iter = client_iter->second.begin();
            while (iter != client_iter->second.end())
            {
                if (fd == *iter){
                    cout<< "client_list del user_id " << fd_iter->second << ",fd " << fd << endl;
                    client_iter->second.erase(iter++);
                }else{
                    iter++;
                }
            }

        }
        Global::fd_map.erase(fd_iter);
        cout << "code is ok" << endl;
    }
    //关闭fd资源。
    UDT::close(fd);
}

void Server::wait() {
    while (1) {
        if( 0 == UDT::epoll_wait( epoll_id, &ready_fd, NULL,-1) ){
            continue;
        }
        for (set<UDTSOCKET>::iterator read_fd_iter = ready_fd.begin(); read_fd_iter != ready_fd.end(); read_fd_iter++) {
            if( *read_fd_iter == serv ){
                this->accept(*read_fd_iter);
            }else{
                //接受 client 消息，入队列，通知线程处理
                //TODO,裁剪成合适的buffer大小再传递给线程。
                shared_ptr<char> msg( new char[MAXLINE]() );
                if (UDT::ERROR == UDT::recvmsg(*read_fd_iter, msg.get(), MAXLINE) ) {
                    //发生错误，关闭fd
                    cout << "recv error:" << UDT::getlasterror().getErrorMessage() << endl;
                    this->close_fd(*read_fd_iter);
                }else{
                    {
                        msg_data msg_d(msg,*read_fd_iter);
                        std::lock_guard<std::mutex> lock(Global::text_mutex);
                        Global::msg_data_deq.push_back(msg_d);
                    }
                    Global::text_cv.notify_one();
                }
                //sleep(10);

            }
        }
    }
}