![TwoBot](https://socialify.git.ci/TwoBotFramework/TwoBot/image?description=1&font=KoHo&forks=1&issues=1&language=1&logo=https%3A%2F%2Favatars.githubusercontent.com%2Fu%2F110024281%3Fs%3D200%26v%3D4&name=1&pattern=Circuit%20Board&pulls=1&stargazers=1&theme=Dark)

### TODO:
+ [x] 建立项目
+ [x] 实现大致框架
+ [x] 引入需要使用的第三方库
+ [ ] 支持onebot的所有功能
    - [x] 实现Onebot HttpAPI的调用
    - [ ] 实现Onebot WebSocket接收消息
+ [ ] 书写完善的文档

### FAQ
1. 为什么用WebSocket而不是HTTP接收消息？
    答：WebSocket性能更高，而httpapi性能更差。
2. 有哪些支持onebot的协议实现？
    答：目前支持onebot的协议库有 oicq, go-cqhttp，mirai等

### 鸣谢
- [MIT] Onebot 标准文档:    [onebot](https://github.com/botuniverse/onebot-11)
- [MIT] C++最好用的Http库:  [cpp-httplib](https://github.com/yhirose/cpp-httplib/)
- [MIT] C++最好用的Json库:  [json](https://github.com/nlohmann/json)
- [Unknown] WebSocket++    [WebSocket++](https://github.com/zaphoyd/websocketpp)
- [Unknown] 微软的Copilot:  [Copilot](https://copilot.github.com/)