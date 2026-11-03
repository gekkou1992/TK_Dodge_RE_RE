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
		else if (msg->type == SKSE::MessagingInterface::kPostLoadGame) {
			animEventHandler::GetSingleton().Register();
		}
		else if (msg->type == SKSE::MessagingInterface::kNewGame) {
			animEventHandler::GetSingleton().Register();
		}
	}
}
