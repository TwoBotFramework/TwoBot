#pragma once
#include <string>
#include <string_view>
#include <optional>
#include <memory>

namespace twobot {
    struct Config{
        std::string url;
        std::uint16_t  port;
        std::optional<std::string> token;
    };

    // Api集合，所有对机器人调用的接口都在这里
    struct ApiSet{
        bool TestHttpConnection();
//TODO: 实现ApiSet
    };

    /// BotInstance是一个机器人实例，机器人实例必须通过BotInstance::createInstance()创建
    /// 因为采用了unique_ptr，所以必须通过std::move传递，可以99.99999%避免内存泄漏
    struct BotInstance{
        static std::unique_ptr<BotInstance> createInstance(const Config &config);
        ApiSet& getApiSet();
    protected:
        ApiSet apiSet;
        explicit BotInstance(const Config &config);
        ~BotInstance() = default;
        friend std::unique_ptr<BotInstance> std::make_unique<BotInstance>(const Config &config);
        friend std::default_delete<BotInstance>;
    };
};