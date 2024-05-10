#include "twobot.hh"
#include <nlohmann/json.hpp>
#include <cstdint>
#include <exception>
#include <iostream>
#include <memory>
#include <string>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#endif

#include <httplib.h>
#include <nlohmann/json.hpp>
#include <brynet/base/Packet.hpp>
#include <brynet/net/http/WebSocketFormat.hpp>
#include <brynet/net/wrapper/HttpServiceBuilder.hpp>
#include <brynet/net/wrapper/ServiceBuilder.hpp>

namespace twobot {
	std::unique_ptr<BotInstance> BotInstance::createInstance(const Config& config) {
		return std::unique_ptr<BotInstance>(new BotInstance{config} );
	}

	ApiSet& BotInstance::getApiSet() {
		return this->apiSet;
	}

	BotInstance::BotInstance(const Config& config) :
		apiSet(ApiSet{ config })
	{

	}

	template<class EventType>
	void BotInstance::onEvent(std::function<void(const EventType&)> callback) {
		EventType event{};
		this->event_callbacks[event.getType()] = Callback([callback](const Event::EventBase& event) {
			try{
				callback(static_cast<const EventType&>(event));
			}catch(const std::exception &e){
				const auto & eventType = event.getType();
				std::cerr << "EventType: {" << eventType.post_type << ", " << eventType.sub_type << "}\n";
				std::cerr << "\tBotInstance::onEvent error: " << e.what() << std::endl;
			}
		});
	}

	void BotInstance::start() {
		using namespace brynet::base;
		using namespace brynet::net;
		using namespace brynet::net::http;
		auto websocket_port = apiSet.config.ws_port;
		auto service = IOThreadTcpService::Create();
		service->startWorkerThread(1);

		auto ws_enter_callback = [this](const HttpSession::Ptr& httpSession,
			WebSocketFormat::WebSocketFrameType opcode,
			const std::string& payload) {
				try {
					nlohmann::json json_payload = nlohmann::json::parse(payload);
					
					// 忽略心跳包
					if(json_payload.contains("meta_event_type"))
						if(json_payload["meta_event_type"] == "heartbeat")
							return;

					auto post_type = (std::string)json_payload["post_type"];

					std::string sub_type;
					if (post_type == "message")
						sub_type = (std::string)json_payload["message_type"];
					else if(post_type == "meta_event")
						sub_type = (std::string)json_payload["sub_type"];
					else if(post_type == "notice")
						sub_type = (std::string)json_payload["notice_type"];

					EventType event_type = {
						post_type,
						sub_type
					};

					auto event = Event::EventBase::construct(event_type);
					if (!event)
						return;
					event->raw_msg = nlohmann::json::parse(payload);
					event->parse();

					if (event_callbacks.count(event_type) != 0) {
						event_callbacks[event_type](
							reinterpret_cast<const Event::EventBase&>(
								*event
							)
						);
					}
				}
				catch (const std::exception& e) {
					std::cerr << "WebSocket CallBack Exception: " << e.what() << std::endl;
				}
				httpSession->send("");

		};

		wrapper::HttpListenerBuilder listener_builder;
		listener_builder
			.WithService(service)
			.AddSocketProcess([](TcpSocket& socket) {
			socket.setNodelay();
				})
			.WithMaxRecvBufferSize(10240)
			.WithAddr(false, "0.0.0.0", websocket_port)
			.WithEnterCallback([ws_enter_callback](const HttpSession::Ptr& httpSession, HttpSessionHandlers& handlers) {
				handlers.setWSCallback(ws_enter_callback);
				})
			.asyncRun()
			;

		while (true)
		{
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	}


	void _::export_functions() {
		// 仅仅为了导出代码，不要当真，更不要去调用！！
		auto instance = BotInstance::createInstance({
			"http://localhost",
			8080,
			8081,
			std::nullopt
		});

		// 仅仅为了特化onEvent模板
		instance->onEvent<Event::GroupMsg>([](const auto&) {});
		instance->onEvent<Event::PrivateMsg>([](const auto&) {});
		instance->onEvent<Event::EnableEvent>([](const auto&) {});
		instance->onEvent<Event::DisableEvent>([](const auto&) {});
		instance->onEvent<Event::ConnectEvent>([](const auto&) {});
		instance->onEvent<Event::GroupUploadNotice>([](const auto&) {});
		instance->onEvent<Event::GroupAdminNotice>([](const auto&) {});
		instance->onEvent<Event::GroupDecreaseNotice>([](const auto&) {});
		instance->onEvent<Event::GroupInceaseNotice>([](const auto&) {});
		instance->onEvent<Event::GroupBanNotice>([](const auto&) {});
		instance->onEvent<Event::FriendAddNotice>([](const auto&) {});
		instance->onEvent<Event::GroupRecallNotice>([](const auto&) {});
		instance->onEvent<Event::FriendRecallNotice>([](const auto&) {});
		instance->onEvent<Event::GroupNotifyNotice>([](const auto&) {});
	}

	std::unique_ptr<Event::EventBase> Event::EventBase::construct(const EventType& event) {
		if (event.post_type == "message") {
			if (event.sub_type == "group") {
				return std::unique_ptr<Event::EventBase>(new Event::GroupMsg());
			} else if (event.sub_type == "private") {
				return std::unique_ptr<Event::EventBase>(new Event::PrivateMsg());
			}
		} else if (event.post_type == "meta_event") {
			if(event.sub_type == "enable"){
				return std::unique_ptr<Event::EventBase>(new Event::EnableEvent());
			}else if(event.sub_type == "disable"){
				return std::unique_ptr<Event::EventBase>(new Event::DisableEvent());
			}else if(event.sub_type == "connect"){
				return std::unique_ptr<Event::EventBase>(new Event::ConnectEvent());
			}
		} else if(event.post_type == "notice"){
			if(event.sub_type == "group_upload"){
				return std::unique_ptr<Event::EventBase>(new Event::GroupUploadNotice());
			} else if (event.sub_type == "group_admin"){
				return std::unique_ptr<Event::EventBase>(new Event::GroupAdminNotice());
			} else if (event.sub_type == "group_decrease"){
				return std::unique_ptr<Event::EventBase>(new Event::GroupDecreaseNotice());
			} else if (event.sub_type == "group_increase"){
				return std::unique_ptr<Event::EventBase>(new Event::GroupInceaseNotice());
			} else if (event.sub_type == "group_ban"){
				return std::unique_ptr<Event::EventBase>( new Event::GroupBanNotice());
			} else if (event.sub_type == "friend_add"){
				return std::unique_ptr<Event::EventBase>(new Event::FriendAddNotice());
			} else if (event.sub_type == "group_recall"){
				return std::unique_ptr<Event::EventBase>(new Event::GroupRecallNotice());
			} else if (event.sub_type == "friend_recall"){
				return std::unique_ptr<Event::EventBase>(new Event::FriendRecallNotice());
			} else if (event.sub_type == "group_notify"){
				return std::unique_ptr<Event::EventBase>(new Event::GroupNotifyNotice());
			}
		}
		return nullptr;
	}

	void Event::GroupMsg::parse() {
        this->time = this->raw_msg["time"];
		this->user_id = raw_msg["user_id"];
		this->self_id = raw_msg["self_id"];
		this->group_id = raw_msg["group_id"];
		this->raw_message = raw_msg["raw_message"];
		// this->group_name = raw_msg["group_name"];
        //this->sub_type = raw_msg["sub_type"]; 
		if(raw_msg["sub_type"] == "normal")
			this->sub_type = NORMAL;
		else if(raw_msg["sub_type"] == "anonymous")
			this->sub_type = ANONYMOUS;
		else if(raw_msg["sub_type"] == "notice")
			this->sub_type = NOTICE;
        this->sender = raw_msg["sender"];
	}

	void Event::PrivateMsg::parse() {
        this->time = this->raw_msg["time"];
		this->user_id = raw_msg["user_id"];
		this->self_id = raw_msg["self_id"];
		this->raw_message = raw_msg["raw_message"];
        // this->sub_type = raw_msg["sub_type"];
		if(raw_msg["sub_type"] == "friend")
			this->sub_type = FRIEND;
		if(raw_msg["sub_type"] == "group")
			this->sub_type = GROUP;
		if(raw_msg["sub_type"] == "other")
			this->sub_type = OTHER;
        this->sender = raw_msg["sender"];
	}

	void Event::EnableEvent::parse(){
		this->time = this->raw_msg["time"];
		this->self_id = raw_msg["self_id"];
	}

	void Event::DisableEvent::parse(){
		this->time = this->raw_msg["time"];
		this->self_id = raw_msg["self_id"];
	}

	void Event::ConnectEvent::parse(){
		this->time = this->raw_msg["time"];
		this->self_id = raw_msg["self_id"];
	}

	void Event::GroupUploadNotice::parse(){
		this->time = this->raw_msg["time"];
		this->self_id = raw_msg["self_id"];
		this->group_id = raw_msg["group_id"];
		this->user_id = raw_msg["user_id"];
		this->file = raw_msg["file"];
	}

	void Event::GroupAdminNotice::parse(){
		this->time = this->raw_msg["time"];
		this->self_id = raw_msg["self_id"];
		this->group_id = raw_msg["group_id"];
		this->user_id = raw_msg["user_id"];
		this->sub_type = ( raw_msg["sub_type"] == "set" ) ? SET : UNSET;
	}

	void Event::GroupDecreaseNotice::parse(){
		this->time = this->raw_msg["time"];
		this->self_id = raw_msg["self_id"];
		this->group_id = raw_msg["group_id"];
		this->user_id = raw_msg["user_id"];
		this->operator_id = raw_msg["operator_id"];
		if(raw_msg["sub_type"] == "leave")
			this->sub_type = LEAVE;
		else if(raw_msg["sub_type"] == "kick")
			this->sub_type = KICK;
		else 
			this->sub_type = KICK_ME;
	}

	void Event::GroupInceaseNotice::parse(){
		this->time = this->raw_msg["time"];
		this->self_id = raw_msg["self_id"];
		this->group_id = raw_msg["group_id"];
		this->user_id = raw_msg["user_id"];
		this->operator_id = raw_msg["operator_id"];
		if(raw_msg["sub_type"] == "approve")
			this->sub_type = APPROVE;
		else if(raw_msg["sub_type"] == "invite")
			this->sub_type = INVITE;
	}

	void Event::GroupBanNotice::parse(){
		this->time = this->raw_msg["time"];
		this->self_id = raw_msg["self_id"];
		this->group_id = raw_msg["group_id"];
		this->user_id = raw_msg["user_id"];
		this->operator_id = raw_msg["operator_id"];
		this->duration = raw_msg["duration"];
		this->sub_type = ( raw_msg["sub_type"] == "ban" ) ? BAN : LIFT_BAN;
	}

	void Event::FriendAddNotice::parse(){
		this->time = this->raw_msg["time"];
		this->self_id = raw_msg["self_id"];
		this->user_id = raw_msg["user_id"];
	}
	
	void Event::FriendRecallNotice::parse(){
		this->time = this->raw_msg["time"];
		this->self_id = raw_msg["self_id"];
		this->user_id = raw_msg["user_id"];
		this->message_id = raw_msg["message_id"];
	}

	void Event::GroupRecallNotice::parse(){
		this->time = this->raw_msg["time"];
		this->self_id = raw_msg["self_id"];
		this->group_id = raw_msg["group_id"];
		this->message_id = raw_msg["message_id"];
		this->operator_id = raw_msg["operator_id"];
		this->user_id = raw_msg["user_id"];
	}

	void Event::GroupNotifyNotice::parse(){
		this->time = this->raw_msg["time"];
		this->self_id = raw_msg["self_id"];
		this->user_id = raw_msg["user_id"];
		this->group_id = raw_msg["group_id"];
		if(raw_msg["sub_type"] == "poke"){
			this->sub_type = POKE;
			this->target_id = raw_msg["target_id"];
		} else if(raw_msg["sub_type"] == "lucky_king"){
			this->sub_type = LUCKY_KING;
			this->target_id = raw_msg["target_id"];
		} else{
			this->sub_type = HONOR;
			if(raw_msg["honor_type"] == "talkative")
				this->honor_type = TALKATIVE;
			else if(raw_msg["honor_type"] == "performer")
				this->honor_type = PERFORMER;
			else if(raw_msg["honor_type"] == "emotion")
				this->honor_type = EMOTION;
		}
	}
};
