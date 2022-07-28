#include <process.h>
#include <twobot.hh>
#include <iostream>

int main() {
    using twobot::Config;
    using twobot::BotInstance;
    using twobot::ApiSet;
    using twobot::Event;

    system("chcp 65001 && cls");

    auto instance = BotInstance::createInstance(Config{
        "127.0.0.1",
        8080,
        8081,
        std::nullopt
    });


    if(!instance->getApiSet().testHttpConnection()){
        std::cerr << "测试连接失败，请启动onebot服务器！" << std::endl;
    }

    instance->onEvent<Event::GroupMsg>([](const Event::GroupMsg & msg){
        std::cerr << "群消息原始格式" << msg.raw_msg << std::endl;
    });

    instance->onEvent<Event::PrivateMsg>([](const Event::PrivateMsg & msg){
        std::cerr << "私聊消息原始格式" << msg.raw_msg << std::endl;
    });


    // 启动机器人
    instance->start();
    
    return 0;
}
