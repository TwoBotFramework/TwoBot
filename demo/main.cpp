#include <process.h>
#include <twobot.hh>
#include <iostream>

/// 警告: 这个项目使用了JSON for modern C++, 必须使用UTF8编码，不然会出现异常。
/// warning: this project uses JSON for modern C++, must use UTF8 encoding, otherwise it will throw exception.

int main() {
    using twobot::Config;
    using twobot::BotInstance;
    using twobot::ApiSet;
    using namespace twobot::Event;

    // 解决UTF8编码，中文乱码问题，不需要可以不加
    system("chcp 65001 && cls");

    auto instance = BotInstance::createInstance(Config{
        .host       = "127.0.0.1",
        .api_port   = 8080,
        .ws_port    = 8081,
        .token      = std::nullopt
    });


    if(!instance->getApiSet().testConnection()){
        std::cerr << "测试连接失败，请启动onebot服务器，并配置HTTP端口！" << std::endl;
    }

    instance->onEvent<GroupMsg>([&instance](const GroupMsg & msg){
        if(msg.raw_message == "你好")
            instance->getApiSet().sendGroupMsg(msg.group_id, "你好，我是twobot！");
        else if(msg.raw_message.find("AT我") != std::string::npos){
            std::string at = "[CQ:at,qq=" + std::to_string(msg.user_id) + "]";
            instance->getApiSet().sendGroupMsg(msg.group_id, at + "要我at你干啥？");
        }
    });

    instance->onEvent<PrivateMsg>([&instance](const PrivateMsg & msg){
        if(msg.raw_message == "你好")
            instance->getApiSet().sendPrivateMsg(msg.user_id, "你好，我是twobot！");
        
    });

    instance->onEvent<EnableEvent>([&instance](const EnableEvent & msg){
        std::cout << "twobot已启动！机器人QQ："<< msg.self_id << std::endl;
    });

    instance->onEvent<DisableEvent>([&instance](const DisableEvent & msg){
        std::cout << "twobot已停止！" << std::endl;
    });

    instance->onEvent<ConnectEvent>([&instance](const ConnectEvent & msg){
        std::cout << "twobot已连接！" << std::endl;
    });


    // 启动机器人
    instance->start();
    
    return 0;
}
