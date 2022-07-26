#include <twobot.hh>
#include <iostream>

int main() {
    using twobot::Config;
    using twobot::BotInstance;
    using twobot::ApiSet;
    using twobot::Event;

    auto instance = BotInstance::createInstance(Config{
        "http://localhost",
        8080,
        8081,
        std::nullopt
    });

    if(!instance->getApiSet().testHttpConnection()){
        std::cerr << "测试连接失败，请启动onebot服务器！" << std::endl;
    }

    instance->onEvent<Event::GroupMsg>([](const Event::GroupMsg & msg){
        std::cerr << msg.raw_msg << std::endl;
    });
    
    return 0;
}
