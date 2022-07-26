#include <twobot.hh>
#include <httplib.h>

namespace twobot {
    
    bool ApiSet::testHttpConnection() {
        httplib::Client client(config.url, config.api_port);
        httplib::Headers headers = {
            {"Content-Type", "application/json"}
        };
        if(config.token.has_value()) {
            headers.insert(
                std::make_pair(
                    std::string{"Authorization"} ,
                    "Bearer " + *config.token
                    )
            );
        }

        auto response = client.Get("/get_version_info", headers);
        return response->status == 200;
    }

    ApiSet::ApiSet(const Config & config) : config(config){

    }

    ApiSet::ApiResult ApiSet::callApi(const std::string &api_name, const nlohmann::json &data) {
        ApiResult result{false, {}};
        httplib::Client client(config.url, config.api_port);
         httplib::Headers headers = {
            {"Content-Type", "application/json"}
         };
        if(config.token.has_value()) {
            headers.insert(
                std::make_pair(
                    std::string{"Authorization"} ,
                    "Bearer " + *config.token
                    )
            );
        }
        httplib::Params params = data;
        auto http_result= client.Post(api_name, headers, params);
        if(http_result->status == 200) {
            result.first = true;
        }else{
            result.first = false;
        }

        try{
            result.second = nlohmann::json::parse(http_result->body);
        }catch(const std::exception &e){
            result.second = nlohmann::json{
                {"error",e.what()}
            };
        }
        return result;
    }

    ApiSet::ApiResult ApiSet::sendPrivateMsg(uint32_t user_id, const std::string &message, bool auto_escape){
        nlohmann::json data = {
            {"user_id", user_id},
            {"message", message},
            {"auto_escape", auto_escape}
        };
        return callApi("/send_private_msg", data);
    }

    ApiSet::ApiResult ApiSet::sendGroupMsg(uint32_t group_id, const std::string &message, bool auto_escape){
        nlohmann::json data = {
            {"group_id", group_id},
            {"message", message},
            {"auto_escape", auto_escape}
        };
        return callApi("/send_group_msg", data);
    }

    ApiSet::ApiResult ApiSet::sendMsg(std::string message_type, uint32_t user_id, uint32_t group_id, const std::string &message, bool auto_escape){
        nlohmann::json data = {
            {"message_type", message_type},
            {"user_id", user_id},
            {"group_id", group_id},
            {"message", message},
            {"auto_escape", auto_escape}
        };
        return callApi("/send_msg", data);
    }

    ApiSet::ApiResult ApiSet::deleteMsg(uint32_t message_id){
        nlohmann::json data = {
            {"message_id", message_id}
        };
        return callApi("/delete_msg", data);
    }

    ApiSet::ApiResult ApiSet::getMsg(uint32_t message_id){
        nlohmann::json data = {
            {"message_id", message_id}
        };
        return callApi("/get_msg", data);
    }

    ApiSet::ApiResult ApiSet::getForwardMsg(uint32_t id){
        nlohmann::json data = {
            {"id", id}
        };
        return callApi("/get_forward_msg", data);
    }

    ApiSet::ApiResult ApiSet::sendLike(uint32_t user_id, uint32_t times){
        nlohmann::json data = {
            {"user_id", user_id},
            {"times", times}
        };
        return callApi("/send_like", data);
    }

    ApiSet::ApiResult ApiSet::setGroupKick(uint32_t group_id, uint32_t user_id, bool reject_add_request){
        nlohmann::json data = {
            {"group_id", group_id},
            {"user_id", user_id},
            {"reject_add_request", reject_add_request}
        };
        return callApi("/set_group_kick", data);
    }

    ApiSet::ApiResult ApiSet::setGroupBan(uint32_t group_id, uint32_t user_id, uint32_t duration){
        nlohmann::json data = {
            {"group_id", group_id},
            {"user_id", user_id},
            {"duration", duration}
        };
        return callApi("/set_group_ban", data);
    }

    ApiSet::ApiResult ApiSet::setGroupAnonymousBan(uint32_t group_id, const std::string& anonymous, const std::string& flag, uint32_t duration){
        nlohmann::json data = {
            {"group_id", group_id},
            {"anonymous", anonymous},
            {"flag", flag},
            {"duration", duration}
        };
        return callApi("/set_group_anonymous_ban", data);
    }

    //ApiResult setGroupWholeBan(uint32_t group_id, bool enable = true);
    ApiSet::ApiResult ApiSet::setGroupWholeBan(uint32_t group_id, bool enable){
        nlohmann::json data = {
            {"group_id", group_id},
            {"enable", enable}
        };
        return callApi("/set_group_whole_ban", data);
    }

    ApiSet::ApiResult ApiSet::setGroupAdmin(uint32_t group_id, uint32_t user_id, bool enable){
        nlohmann::json data = {
            {"group_id", group_id},
            {"user_id", user_id},
            {"enable", enable}
        };
        return callApi("/set_group_admin", data);
    }

    ApiSet::ApiResult ApiSet::setGroupAnonymous(uint32_t group_id, bool enable){
        nlohmann::json data = {
            {"group_id", group_id},
            {"enable", enable}
        };
        return callApi("/set_group_anonymous", data);
    }

    ApiSet::ApiResult ApiSet::setGroupCard(uint32_t group_id, uint32_t user_id, const std::string& card){
        nlohmann::json data = {
            {"group_id", group_id},
            {"user_id", user_id},
            {"card", card}
        };
        return callApi("/set_group_card", data);
    }

    // ApiResult setGroupName(uint32_t group_id, const std::string& name);
    ApiSet::ApiResult ApiSet::setGroupName(uint32_t group_id, const std::string& name){
        nlohmann::json data = {
            {"group_id", group_id},
            {"name", name}
        };
        return callApi("/set_group_name", data);
    }

    // ApiResult setGroupLeave(uint32_t group_id, bool is_dismiss = false);
    ApiSet::ApiResult ApiSet::setGroupLeave(uint32_t group_id, bool is_dismiss){
        nlohmann::json data = {
            {"group_id", group_id},
            {"is_dismiss", is_dismiss}
        };
        return callApi("/set_group_leave", data);
    }

    // ApiResult setGroupSpecialTitle(uint32_t group_id, uint32_t user_id, const std::string& special_title, int32_t duration = -1); 
    ApiSet::ApiResult ApiSet::setGroupSpecialTitle(uint32_t group_id, uint32_t user_id, const std::string& special_title, int32_t duration){
        nlohmann::json data = {
            {"group_id", group_id},
            {"user_id", user_id},
            {"special_title", special_title},
            {"duration", duration}
        };
        return callApi("/set_group_special_title", data);
    }

    // ApiResult setFriendAddRequest(const std::string& flag, bool approve = true, const std::string& remark = "");
    ApiSet::ApiResult ApiSet::setFriendAddRequest(const std::string& flag, bool approve, const std::string& remark){
        nlohmann::json data = {
            {"flag", flag},
            {"approve", approve},
            {"remark", remark}
        };
        return callApi("/set_friend_add_request", data);
    }

    // ApiResult setGroupAddRequest(const std::string& flag, const std::string& sub_type, bool approve = true, const std::string& reason = "");
    ApiSet::ApiResult ApiSet::setGroupAddRequest(const std::string& flag, const std::string& sub_type, bool approve, const std::string& reason){
        nlohmann::json data = {
            {"flag", flag},
            {"sub_type", sub_type},
            {"approve", approve},
            {"reason", reason}
        };
        return callApi("/set_group_add_request", data);
    }

    // ApiResult getLoginInfo();
    ApiSet::ApiResult ApiSet::getLoginInfo(){
        return callApi("/get_login_info", {});
    }

    // ApiResult getStrangerInfo(uint32_t user_id, bool no_cache = false);
    ApiSet::ApiResult ApiSet::getStrangerInfo(uint32_t user_id, bool no_cache){
        nlohmann::json data = {
            {"user_id", user_id},
            {"no_cache", no_cache}
        };
        return callApi("/get_stranger_info", data);
    }

    // ApiResult getFriendList();
    ApiSet::ApiResult ApiSet::getFriendList(){
        return callApi("/get_friend_list", {});
    }

    // ApiResult getGroupInfo(uint32_t group_id, bool no_cache = false);
    ApiSet::ApiResult ApiSet::getGroupInfo(uint32_t group_id, bool no_cache){
        nlohmann::json data = {
            {"group_id", group_id},
            {"no_cache", no_cache}
        };
        return callApi("/get_group_info", data);
    }

    // ApiResult getGroupList();
    ApiSet::ApiResult ApiSet::getGroupList(){
        return callApi("/get_group_list", {});
    }

    // ApiResult getGroupMemberInfo(uint32_t group_id, uint32_t user_id, bool no_cache = false);
    ApiSet::ApiResult ApiSet::getGroupMemberInfo(uint32_t group_id, uint32_t user_id, bool no_cache){
        nlohmann::json data = {
            {"group_id", group_id},
            {"user_id", user_id},
            {"no_cache", no_cache}
        };
        return callApi("/get_group_member_info", data);
    }

    // ApiResult getGroupMemberList(uint32_t group_id);
    ApiSet::ApiResult ApiSet::getGroupMemberList(uint32_t group_id){
        nlohmann::json data = {
            {"group_id", group_id}
        };
        return callApi("/get_group_member_list", data);
    }
}