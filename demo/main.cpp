#include <twobot.hh>
#include <iostream>

int main() {
    using twobot::Config;
    using twobot::BotInstance;
    using twobot::ApiSet;

    auto instance = BotInstance::createInstance(Config{
        "http://localhost",
        8080,
        std::nullopt
    });
    if(!instance->getApiSet().TestHttpConnection()){
        std::cerr << "测试连接失败，请启动onebot服务器！" << std::endl;
    }
    return 0;
}
