#include "twobot.hh"

namespace twobot{
  std::unique_ptr<BotInstance> BotInstance::createInstance(const Config &config) {
        return std::make_unique<BotInstance>(config);
  }

    ApiSet &BotInstance::getApiSet() {
        return this->apiSet;
    }

    BotInstance::BotInstance(const Config &config) :
        apiSet(ApiSet{
            //TODO: 创建初始化ApiSet的代码
        })
    {

    }
};
