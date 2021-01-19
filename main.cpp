#include <iostream>
#include <cstring>
#include "Global.h"
#include "Login.h"
#include "Text.h"
#include "Server.h"
#include "Contact.h"
#include <unistd.h>
#include <thread>
using namespace std;

//处理消息线程
void msg_deal(int i)
{
    for (;;) {
        std::unique_lock<std::mutex> lock(Global::text_mutex);
        // 等待主线程发送数据。
        while (Global::msg_data_deq.empty()){
            Global::text_cv.wait(lock);
        }
        //读取队列消息。
        msg_data msg_d = Global::msg_data_deq.front();
        Global::msg_data_deq.pop_front();
        lock.unlock();

        Msg::MsgType msg_type;
        msg_type.ParseFromArray(msg_d.first.get(),MAXLINE);
        if( Msg::TYPE_LOGIN_Q == msg_type.value() ){
            Login login(&msg_d);
            login.go();
        }
        else if( Msg::TYPE_TEXT == msg_type.value() ){
            Text text(&msg_d);
            text.send();
        }
    }
}

int main() {

    /* 开启socket监听 */
    Server server;
    if(server.listen()){
        cout << "server listen fail" << UDT::getlasterror().getErrorMessage() << endl;
        //listen 失败 退出。
        return 1;
    };

    /* 开启多线程处理消息 */
    thread msg_thread[NUMTHREAD];
    for (int i = 0; i < NUMTHREAD;i++)
    {
        msg_thread[i] = thread(msg_deal,i);
    }

    server.wait();
    return 0;
}
