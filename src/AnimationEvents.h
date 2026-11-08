#pragma once

class animEventHandler :
	public RE::BSTEventSink<RE::BSAnimationGraphEvent>
{
public:
	animEventHandler();
	animEventHandler(const animEventHandler&) = delete;
	animEventHandler(animEventHandler&&) = delete;
	~animEventHandler() override = default;

	animEventHandler& operator=(const animEventHandler&) = delete;
	animEventHandler& operator=(animEventHandler&&) = delete;
	enum EVENT_INDEX : std::uint32_t
	{
		NO_EVENT = 0x00,
		ANIMGRAPH_ADD = 0x01,
		ANIMGRAPH_REMOVE = 0x02
	};
	static animEventHandler& GetSingleton(void);
	void Register(void);
private:
	RE::BSEventNotifyControl ProcessEvent(const RE::BSAnimationGraphEvent* a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>*) override;
	void EventControl(const std::uint32_t index);
	std::uint32_t toggleEvents;
};

