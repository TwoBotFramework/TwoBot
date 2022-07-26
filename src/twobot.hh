#pragma once
#include "nlohmann/json_fwd.hpp"
#include <string>
#include <string_view>
#include <optional>
#include <memory>
#include <unordered_map>
#include <functional>
#include <nlohmann/json.hpp>
#include <utility>


namespace twobot
{
    struct EventType{
        std::string_view type;
        std::string_view sub_type;

        bool operator==(const EventType &other) const {
            return type == other.type && sub_type == other.sub_type;
        }
    };
};

namespace std{
    template<>
    struct hash<twobot::EventType> {
        size_t operator()(const twobot::EventType &event) const {
            auto left_hash = hash<std::string_view>()(event.type);
            auto right_hash = hash<std::string_view>()(event.sub_type);
            return  left_hash ^ (right_hash << 1);
        }
    };
};

namespace twobot {

    // 服务器配置
    struct Config{
        std::string url;
        std::uint16_t  api_port;
        std::uint16_t  ws_port;
        std::optional<std::string> token;
    };


    // Api集合，所有对机器人调用的接口都在这里
    struct ApiSet{
        bool testHttpConnection();
//TODO: 实现ApiSet
        // 万api之母，负责提起所有的api的请求
        using ApiResult = std::pair<bool, nlohmann::json>;
        ApiResult callApi(const std::string &api_name, const nlohmann::json &data);

        // 下面要实现onebot标准的所有api

        /** 
        send_private_msg 发送私聊消息
        参数
            字段名	数据类型	默认值	说明
            user_id	number	-	对方 QQ 号
            message	message	-	要发送的内容
            auto_escape	boolean	false	消息内容是否作为纯文本发送（即不解析 CQ 码），只在 message 字段是字符串时有效
        响应数据
            字段名	数据类型	说明
            message_id	number (int32)	消息 ID
        */
        ApiResult sendPrivateMsg(uint32_t user_id, const std::string &message, bool auto_escape = false);
        
        /**
        send_group_msg 发送群消息
        参数
            字段名	数据类型	默认值	说明
            group_id	number	-	群号
            message	message	-	要发送的内容
            auto_escape	boolean	false	消息内容是否作为纯文本发送（即不解析 CQ 码），只在 message 字段是字符串时有效
        响应数据
            字段名	数据类型	说明
            message_id	number (int32)	消息 ID
        */
        ApiResult sendGroupMsg(uint32_t group_id, const std::string &message, bool auto_escape = false);
        
        /**
        send_msg 发送消息
        参数
            字段名	数据类型	默认值	说明
            message_type	string	-	消息类型，支持 private、group，分别对应私聊、群组，如不传入，则根据传入的 *_id 参数判断
            user_id	number	-	对方 QQ 号（消息类型为 private 时需要）
            group_id	number	-	群号（消息类型为 group 时需要）
            message	message	-	要发送的内容
            auto_escape	boolean	false	消息内容是否作为纯文本发送（即不解析 CQ 码），只在 message 字段是字符串时有效
        响应数据
            字段名	数据类型	说明
            message_id	number (int32)	消息 ID
        */
        ApiResult sendMsg(std::string message_type, uint32_t user_id, uint32_t group_id, const std::string &message, bool auto_escape = false);
        
        /**
        delete_msg 撤回消息
        参数
            字段名	数据类型	默认值	说明
            message_id	number (int32)	-	消息 ID
        响应数据
        无
        */
        ApiResult deleteMsg(uint32_t message_id);
        
        /**
        get_msg 获取消息
        参数
            字段名	数据类型	说明
            message_id	number (int32)	消息 ID
        响应数据
            字段名	数据类型	说明
            time	number (int32)	发送时间
            message_type	string	消息类型，同 消息事件
            message_id	number (int32)	消息 ID
            real_id	number (int32)	消息真实 ID
            sender	object	发送人信息，同 消息事件
            message	message	消息内容
        */
        ApiResult getMsg(uint32_t message_id);
        
        /**
        get_forward_msg 获取合并转发消息
        参数
            字段名	数据类型	说明
            id	string	合并转发 ID
        响应数据
            字段名	类型	说明
            message	message	消息内容，使用 消息的数组格式 表示，数组中的消息段全部为 node 消息段
        */
        ApiResult getForwardMsg(uint32_t id);

        /**
        send_like 发送好友赞
        参数
            字段名	数据类型	默认值	说明
            user_id	number	-	对方 QQ 号
            times	number	1	赞的次数，每个好友每天最多 10 次
        响应数据
            无
        **/
        ApiResult sendLike(uint32_t user_id, uint32_t times = 1);

        /**
        set_group_kick 群组踢人
        参数
            字段名	数据类型	默认值	说明
            group_id	number	-	群号
            user_id	number	-	要踢的 QQ 号
            reject_add_request	boolean	false	拒绝此人的加群请求
        响应数据
            无
        */
        ApiResult setGroupKick(uint32_t group_id, uint32_t user_id, bool reject_add_request = false);

        /**
        set_group_ban 群组单人禁言
        参数
            字段名	数据类型	默认值	说明
            group_id	number	-	群号
            user_id	number	-	要禁言的 QQ 号
            duration	number	30 * 60	禁言时长，单位秒，0 表示取消禁言
        响应数据
            无
        */
        ApiResult setGroupBan(uint32_t group_id, uint32_t user_id, uint32_t duration = 30 * 60);

        /** 
        set_group_anonymous_ban 群组匿名用户禁言
        参数
            字段名	数据类型	默认值	说明
            group_id	number	-	群号
            anonymous	object	-	可选，要禁言的匿名用户对象（群消息上报的 anonymous 字段）
            anonymous_flag 或 flag	string	-	可选，要禁言的匿名用户的 flag（需从群消息上报的数据中获得）
            duration	number	30 * 60	禁言时长，单位秒，无法取消匿名用户禁言
        上面的 anonymous 和 anonymous_flag 两者任选其一传入即可，若都传入，则使用 anonymous。

        响应数据
            无
        */
        ApiResult setGroupAnonymousBan(uint32_t group_id, const std::string& anonymous, const std::string& flag, uint32_t duration = 30 * 60);

        /** 
        set_group_whole_ban 群组全员禁言
        参数
            字段名	数据类型	默认值	说明
            group_id	number	-	群号
            enable	boolean	true	是否禁言
        响应数据
            无
        */
        ApiResult setGroupWholeBan(uint32_t group_id, bool enable = true);

        /**
        set_group_admin 群组设置管理员
        参数
            字段名	数据类型	默认值	说明
            group_id	number	-	群号
            user_id	number	-	要设置的管理员 QQ 号
            enable	boolean	true	是否设置为管理员
        响应数据
            无
        */
        ApiResult setGroupAdmin(uint32_t group_id, uint32_t user_id, bool enable = true);

        /**
        set_group_anonymous 群组匿名设置
        参数
            字段名	数据类型	默认值	说明
            group_id	number	-	群号
            enable	boolean	true	是否允许匿名聊天
        响应数据
            无
        */
        ApiResult setGroupAnonymous(uint32_t group_id, bool enable = true);

        /**
        set_group_card 群名片设置（群备注）
        参数
            字段名	数据类型	默认值	说明
            group_id	number	-	群号
            user_id	number	-	要设置的群名片 QQ 号
            card	string	-	要设置的群名片内容
        响应数据
            无
        */
        ApiResult setGroupCard(uint32_t group_id, uint32_t user_id, const std::string& card);

        /**
        set_group_name 设置群名
        参数
            字段名	数据类型	默认值	说明
            group_id	number	-	群号
            name	string	-	要设置的群名
        响应数据
            无
        */
        ApiResult setGroupName(uint32_t group_id, const std::string& name);

        /**
        set_group_leave 退出群组
        参数
            字段名	数据类型	默认值	说明
            group_id	number	-	群号
            is_dismiss	boolean	false	是否解散该群组
        响应数据
            无
        */
        ApiResult setGroupLeave(uint32_t group_id, bool is_dismiss = false);


        /**
        set_group_special_title 设置群组专属头衔
        参数
            字段名	数据类型	默认值	说明
            group_id	number	-	群号
            user_id	number	-	要设置的 QQ 号
            special_title	string	空	专属头衔，不填或空字符串表示删除专属头衔
            duration	number	-1	专属头衔有效期，单位秒，-1 表示永久，不过此项似乎没有效果，可能是只有某些特殊的时间长度有效，有待测试
        响应数据
            无
        */
        ApiResult setGroupSpecialTitle(uint32_t group_id, uint32_t user_id, const std::string& special_title, int32_t duration = -1); 

        /**
        set_friend_add_request 处理加好友请求
        参数
            字段名	数据类型	默认值	说明
            flag	string	-	加好友请求的 flag（需从上报的数据中获得）
            approve	boolean	true	是否同意请求
            remark	string	空	添加后的好友备注（仅在同意时有效）
        响应数据
            无
        */
        ApiResult setFriendAddRequest(const std::string& flag, bool approve = true, const std::string& remark = "");

        /**
        set_group_add_request 处理加群请求／邀请
        参数
            字段名	数据类型	默认值	说明
            flag	string	-	加群请求的 flag（需从上报的数据中获得）
            sub_type 或 type	string	-	add 或 invite，请求类型（需要和上报消息中的 sub_type 字段相符）
            approve	boolean	true	是否同意请求／邀请
            reason	string	空	拒绝理由（仅在拒绝时有效）
        响应数据
            无
        */
        ApiResult setGroupAddRequest(const std::string& flag, const std::string& sub_type, bool approve = true, const std::string& reason = "");

        /**
        get_login_info 获取登录号信息
        参数
            无

        响应数据
            字段名	数据类型	说明
            user_id	number (int64)	QQ 号
            nickname	string	QQ 昵称
        */
        ApiResult getLoginInfo();

        /**
        get_stranger_info 获取陌生人信息
        参数
            字段名	数据类型	默认值	说明
            user_id	number	-	QQ 号
            no_cache	boolean	false	是否不使用缓存（使用缓存可能更新不及时，但响应更快）
        响应数据
            字段名	数据类型	说明
            user_id	number (int64)	QQ 号
            nickname	string	昵称
            sex	string	性别，male 或 female 或 unknown
            age	number (int32)	年龄
        */
        ApiResult getStrangerInfo(uint32_t user_id, bool no_cache = false);

        /**
        get_friend_list 获取好友列表
        参数
            无

        响应数据
            响应内容为 JSON 数组，每个元素如下：
            字段名	数据类型	说明
            user_id	number (int64)	QQ 号
            nickname	string	昵称
            remark	string	备注名
        */
        ApiResult getFriendList();

        /**
        get_group_info 获取群信息
        参数
            字段名	数据类型	默认值	说明
            group_id	number	-	群号
            no_cache	boolean	false	是否不使用缓存（使用缓存可能更新不及时，但响应更快）
        响应数据
            字段名	数据类型	说明
            group_id	number (int64)	群号
            group_name	string	群名称
            member_count	number (int32)	成员数
            max_member_count	number (int32)	最大成员数（群容量）
        */
        ApiResult getGroupInfo(uint32_t group_id, bool no_cache = false);

        /**
        get_group_list 获取群列表
        参数
            无

        响应数据
            响应内容为 JSON 数组，每个元素和上面的 get_group_info 接口相同。
        */
        ApiResult getGroupList();

        /**
        get_group_member_info 获取群成员信息
        参数
            字段名	数据类型	默认值	说明
            group_id	number	-	群号
            user_id	number	-	QQ 号
            no_cache	boolean	false	是否不使用缓存（使用缓存可能更新不及时，但响应更快）
        响应数据
            字段名	数据类型	说明
            group_id	number (int64)	群号
            user_id	number (int64)	QQ 号
            nickname	string	昵称
            card	string	群名片／备注
            sex	string	性别，male 或 female 或 unknown
            age	number (int32)	年龄
            area	string	地区
            join_time	number (int32)	加群时间戳
            last_sent_time	number (int32)	最后发言时间戳
            level	string	成员等级
            role	string	角色，owner 或 admin 或 member
            unfriendly	boolean	是否不良记录成员
            title	string	专属头衔
            title_expire_time	number (int32)	专属头衔过期时间戳
            card_changeable	boolean	是否允许修改群名片
        */
        ApiResult getGroupMemberInfo(uint32_t group_id, uint32_t user_id, bool no_cache = false);

        /**
        get_group_member_list 获取群成员列表
        参数
            字段名	数据类型	默认值	说明
            group_id	number (int64)	-	群号
        响应数据
            响应内容为 JSON 数组，每个元素的内容和上面的 get_group_member_info 接口相同，但对于同一个群组的同一个成员，获取列表时和获取单独的成员信息时，某些字段可能有所不同，例如 area、title 等字段在获取列表时无法获得，具体应以单独的成员信息为准。
        */
        ApiResult getGroupMemberList(uint32_t group_id);
    protected:
        ApiSet (const Config &config);
        Config config;
        friend class BotInstance;
    };



    struct Event{
        struct EventBase{
            nlohmann::json raw_msg;
            // { message_type , sub_type }
            virtual EventType getType() const = 0;
        };

        // 以 以下类为模板参数 的onEvent必须在export_functions中调用一次，才能实现模板特化导出
        struct PrivateMsg : EventBase{
            EventType getType() const override{
                return {"message", "private"};
            }
        };

        struct GroupMsg : EventBase{
            EventType getType() const override{
                return {"message", "group"};
            }
        };

    };

    /// BotInstance是一个机器人实例，机器人实例必须通过BotInstance::createInstance()创建
    /// 因为采用了unique_ptr，所以必须通过std::move传递，可以99.99999%避免内存泄漏
    struct BotInstance{
        // 消息类型
        // 消息回调函数原型
        using Callback = std::function<void(const Event::EventBase &)>;


        // 创建机器人实例
        static std::unique_ptr<BotInstance> createInstance(const Config &config);
        
        // 获取Api集合
        ApiSet& getApiSet();
        
        // 注册事件监听器
        template<class EventType>
        void onEvent(std::function<void(const EventType &)> callback);


    protected:
        ApiSet apiSet;
        std::unordered_map<EventType, Callback> event_callbacks{};
    protected:
        explicit BotInstance(const Config &config);
        ~BotInstance() = default;
        
        friend std::unique_ptr<BotInstance> std::make_unique<BotInstance>(const Config &config);
        friend std::default_delete<BotInstance>;
    };

    // 存放私有的东西，防止编译器优化掉模板特化
    namespace _{
        // 仅仅为了导出代码，不要当真，更不要去调用！！
        void export_functions();
    };
};
