#pragma once


using EventResult = RE::BSEventNotifyControl;

class InputEventHandler : public RE::BSTEventSink<RE::InputEvent*>
{
public:
	InputEventHandler(const InputEventHandler&) = delete;
	InputEventHandler(InputEventHandler&&) = delete;
	InputEventHandler& operator=(const InputEventHandler&) = delete;
	InputEventHandler& operator=(InputEventHandler&&) = delete;

	EventResult ProcessEvent(RE::InputEvent* const* a_event, RE::BSTEventSource<RE::InputEvent*>* a_eventSource) override;

	static void Register()
	{
		spdlog::info("Register input event...");
		auto deviceManager = RE::BSInputDeviceManager::GetSingleton();
		deviceManager->AddEventSink(&InputEventHandler::GetSingleton());
		spdlog::info("..Done");
	}

private:
	InputEventHandler() = default;	
	~InputEventHandler() override = default;

	static InputEventHandler& GetSingleton()
	{
		static InputEventHandler singleton;
		return singleton;
	}
		
	static std::uint32_t GetGamepadIndex(RE::BSWin32GamepadDevice::Key a_key);
	inline uint32_t getOffsetButtonIDCode(RE::ButtonEvent* a_event);	
};

