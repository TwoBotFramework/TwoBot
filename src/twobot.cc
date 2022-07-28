#include "twobot.hh"
#include "nlohmann/json_fwd.hpp"
#include <cstddef>
#include <exception>
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
		return std::make_unique<BotInstance>(config);
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
			callback(static_cast<const EventType&>(event));
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
					// meta_event_type 暂时不作解析
					if (json_payload.contains("meta_event_type"))
						return;

					auto post_type = (std::string)json_payload["post_type"];

					std::string sub_type;
					if (post_type == "message")
						sub_type = (std::string)json_payload["message_type"];
					else
						sub_type = (std::string)json_payload["sub_type"];

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

		};

		wrapper::HttpListenerBuilder listener_builder;
		listener_builder
			.WithService(service)
			.AddSocketProcess([](TcpSocket& socket) {
			socket.setNodelay();
				})
			.WithMaxRecvBufferSize(1024)
					.WithAddr(false, "0.0.0.0", websocket_port)
					.WithEnterCallback([ws_enter_callback](const HttpSession::Ptr& httpSession, HttpSessionHandlers& handlers) {
					handlers.setWSCallback(ws_enter_callback);
						}).asyncRun()
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
		instance->onEvent<Event::GroupMsg>([](const Event::GroupMsg&) {});
		instance->onEvent<Event::PrivateMsg>([](const Event::PrivateMsg&) {});
	}

	std::unique_ptr<Event::EventBase> Event::EventBase::construct(const EventType& evnet) {
		if (evnet.post_type == "message") {
			if (evnet.message_type == "group") {
				return std::unique_ptr<Event::EventBase>(new Event::GroupMsg());
			}
			else if (evnet.message_type == "private") {
				return std::unique_ptr<Event::EventBase>(new Event::PrivateMsg());
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
		this->group_name = raw_msg["group_name"];
        this->sub_type = raw_msg["sub_type"]; 

        this->sender = raw_msg["sender"];
	}

	void Event::PrivateMsg::parse() {
        this->time = this->raw_msg["time"];
		this->user_id = raw_msg["user_id"];
		this->self_id = raw_msg["self_id"];
		this->raw_message = raw_msg["raw_message"];
        this->sub_type = raw_msg["sub_type"]; 
        this->sender = raw_msg["sender"];
	}

};
