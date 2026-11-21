#pragma once

class animEventHandler :
	public RE::BSTEventSink<RE::BSAnimationGraphEvent>,
	public RE::BSTEventSink<RE::MenuOpenCloseEvent>
{
public:	
	animEventHandler(const animEventHandler&) = delete;
	animEventHandler(animEventHandler&&) = delete;
	animEventHandler& operator=(const animEventHandler&) = delete;
	animEventHandler& operator=(animEventHandler&&) = delete;

	enum EVENT_INDEX : std::uint32_t
	{
		NO_EVENT = 0x00,
		ANIMGRAPH_ADD = 0x01,
		ANIMGRAPH_REMOVE = 0x02,
		MENU_OPEN_CLOSE_ADD = 0x04,
		MENU_OPEN_CLOSE_REMOVE = 0x08
	};
	static animEventHandler& GetSingleton(void);
	void Register(const bool bNewGame);
private:
	animEventHandler() = default;
	~animEventHandler() override = default;
	RE::BSEventNotifyControl ProcessEvent(const RE::BSAnimationGraphEvent* a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>*) override;
	RE::BSEventNotifyControl ProcessEvent(const RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>*) override;
	void EventControl(const std::uint32_t index);	
};

