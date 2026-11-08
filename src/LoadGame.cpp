#include "Hooks.h"
#include "InputEvents.h"
#include "TKRE.h"
#include "AnimationEvents.h"
#include "Menu.h"

namespace TKDodge
{

	void EventCallback(SKSE::MessagingInterface::Message* msg)
	{
		if (msg->type == SKSE::MessagingInterface::kDataLoaded) {			
			tkdrere::Register();
			InputEventHandler::Register();
			Hooks::install();
		}
		else if (msg->type == SKSE::MessagingInterface::kPostLoad) {
			OAR_API::Conditions::GetAPI(OAR_API::Conditions::InterfaceVersion::V2);
			if (g_oarConditionsInterface) {
				switch (OAR_API::Conditions::AddCustomCondition<tkdrere::PrioritizeHorizontalDodgeCondition>()) {
					using enum OAR_API::Conditions::APIResult;
				case OK:
					spdlog::info("Registered {} condition!", tkdrere::PrioritizeHorizontalDodgeCondition::CONDITION_NAME);
					break;
				case AlreadyRegistered:
					spdlog::warn("Condition {} is already registered!", tkdrere::PrioritizeHorizontalDodgeCondition::CONDITION_NAME);
					break;
				case Invalid:
					spdlog::error("Condition {} is invalid!", tkdrere::PrioritizeHorizontalDodgeCondition::CONDITION_NAME);
					break;
				case Failed:
					spdlog::error("Failed to register condition {}!", tkdrere::PrioritizeHorizontalDodgeCondition::CONDITION_NAME);
					break;
				}
			}
			else {
				spdlog::error("Failed to request Open Animation Replacer API");
			}
		}
		else if (msg->type == SKSE::MessagingInterface::kPostLoadGame) {
			animEventHandler::GetSingleton().Register();
		}
		else if (msg->type == SKSE::MessagingInterface::kNewGame) {
			animEventHandler::GetSingleton().Register();
		}
	}
}