#include "twobot.hh"
#include <utility>

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
#endif

#include <httplib.h>
#include <nlohmann/json.hpp>

namespace twobot{
  std::unique_ptr<BotInstance> BotInstance::createInstance(const Config &config) {
        return std::make_unique<BotInstance>(config);
  }

    ApiSet &BotInstance::getApiSet() {
        return this->apiSet;
    }

    BotInstance::BotInstance(const Config &config) :
        apiSet(ApiSet{config})
    {

    }

    template<class EventType>
    void BotInstance::onEvent(std::function<void(const EventType &)> callback){
        EventType event{};
        this->event_callbacks[event.getType()] =  Callback([callback](const Event::EventBase & event){
            callback(static_cast<const EventType&>( event) );
        });
    }

    void _::export_functions(){
        // 仅仅为了导出代码，不要当真，更不要去调用！！
        auto instance = BotInstance::createInstance({
            "http://localhost",
            8080,
            8081,
            std::nullopt
        });
        
        // 仅仅为了特化onEvent模板
        instance->onEvent<Event::GroupMsg>([](const Event::GroupMsg &){});
        instance->onEvent<Event::PrivateMsg>([](const Event::PrivateMsg &){});
    }

};
