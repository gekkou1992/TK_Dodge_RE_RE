#include "LoadGame.h"


SKSEPluginLoad(const SKSE::LoadInterface* a_skse)
{
	SKSE::Init(a_skse);

	// do stuff
	auto g_message = SKSE::GetMessagingInterface();
	if (!g_message) {
		spdlog::error("Messaging Interface Not Found!");
		return false;
	}

	g_message->RegisterListener(TKDodge::EventCallback);

	return true;
}
