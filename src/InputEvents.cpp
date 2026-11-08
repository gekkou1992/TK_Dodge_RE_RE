#include "InputEvents.h"
#include "TKRE.h"
#include "AnimationEvents.h"
#include "Menu.h"

std::uint32_t InputEventHandler::GetGamepadIndex(RE::BSWin32GamepadDevice::Key a_key)
{
	using Key = RE::BSWin32GamepadDevice::Key;

	std::uint32_t index;
	switch (a_key) {
	case Key::kUp:
		index = 0;
		break;
	case Key::kDown:
		index = 1;
		break;
	case Key::kLeft:
		index = 2;
		break;
	case Key::kRight:
		index = 3;
		break;
	case Key::kStart:
		index = 4;
		break;
	case Key::kBack:
		index = 5;
		break;
	case Key::kLeftThumb:
		index = 6;
		break;
	case Key::kRightThumb:
		index = 7;
		break;
	case Key::kLeftShoulder:
		index = 8;
		break;
	case Key::kRightShoulder:
		index = 9;
		break;
	case Key::kA:
		index = 10;
		break;
	case Key::kB:
		index = 11;
		break;
	case Key::kX:
		index = 12;
		break;
	case Key::kY:
		index = 13;
		break;
	case Key::kLeftTrigger:
		index = 14;
		break;
	case Key::kRightTrigger:
		index = 15;
		break;
	default:
		index = tkdrere::ButtonCodeOffset::kInvalid;
		break;
	}

	return index != tkdrere::ButtonCodeOffset::kInvalid ? index + tkdrere::ButtonCodeOffset::kGamepadOffset : tkdrere::ButtonCodeOffset::kInvalid;
}

inline uint32_t InputEventHandler::getOffsetButtonIDCode(RE::ButtonEvent* a_event)
{
	auto id = a_event->GetIDCode();
	using DeviceType = RE::INPUT_DEVICE;
	switch (a_event->GetDevice()) {
	case DeviceType::kMouse:
		id += tkdrere::ButtonCodeOffset::kMouseOffset;
		break;
	case DeviceType::kKeyboard:
		id += tkdrere::ButtonCodeOffset::kKeyboardOffset;
		break;
	case DeviceType::kGamepad:
		id = GetGamepadIndex(static_cast<RE::BSWin32GamepadDevice::Key>(id));
		break;
	}
	return id;
}

EventResult InputEventHandler::ProcessEvent(RE::InputEvent* const* a_event, RE::BSTEventSource<RE::InputEvent*>*)
{
	using EventType = RE::INPUT_EVENT_TYPE;
	
	if (!a_event) 
		return EventResult::kContinue;
	
	const auto ui = RE::UI::GetSingleton();
	if (!ui || ui->GameIsPaused()) {
		return EventResult::kContinue;
	}
	
	const auto ctrlMap = RE::ControlMap::GetSingleton();
	if (!ctrlMap)
		return EventResult::kContinue;

	const auto& contextStack = ctrlMap->GetRuntimeData().contextPriorityStack;
	if (contextStack.empty() || contextStack.back() != RE::UserEvents::INPUT_CONTEXT_ID::kGameplay)
		return EventResult::kContinue;

	const auto& settings = Settings::GetSingleton();
	for (auto event = *a_event; event; event = event->next) {
		if (event->eventType != EventType::kButton)
			continue;
		
		const auto button = static_cast<RE::ButtonEvent*>(event);		
		if (!button->IsDown()) {
			continue;
		}

		if (!button->HasIDCode()) {
			continue;
		}

		const auto keyCode = getOffsetButtonIDCode(button);

		if (keyCode == tkdrere::ButtonCodeOffset::kInvalid)
			continue;
		
		if (keyCode == settings.dodgeKey) {
			TKRE::dodge(false);
		}
		else if (button->GetDevice() == RE::INPUT_DEVICE::kGamepad &&
			settings.GamepadDodgeKey != settings.dodgeKey &&
			keyCode == settings.GamepadDodgeKey) {
			TKRE::dodge(false);
		}
	}

	return EventResult::kContinue;
}