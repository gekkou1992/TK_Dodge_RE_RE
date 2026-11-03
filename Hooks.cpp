#include "Hooks.h"
#include "TKRE.h"
#include "InputEvents.h"
#include "Menu.h"

namespace Hooks
{
	void install()
	{
		spdlog::info("Hooking...");

		SprintHandlerHook::Hook();

		spdlog::info("...success");
	}
	static bool bStoppingSprint = false;
	//static float heldDownTimeOffset = 0.f;

	void SprintHandlerHook::ProcessButton(RE::SprintHandler* a_this, RE::ButtonEvent* a_event, RE::PlayerControlsData* a_data)
	{		
		const auto& settings = Settings::GetSingleton();
		if (settings.EnableSprintKeyDodge) {
			auto playerCharacter = RE::PlayerCharacter::GetSingleton();
			if (!playerCharacter) {
				_ProcessButton(a_this, a_event, a_data);
				return;
			}

			auto userEvent = a_event->QUserEvent();
			auto userEvents = RE::UserEvents::GetSingleton();

			if (settings.EnableSprintKeyDoubleTap) {
				if (userEvent == userEvents->sprint) {
					static std::uint64_t lastTapTime = 0;
					static bool waitingSecondTap = false;
					static bool secondTap = false;

					const auto now = static_cast<std::uint64_t>(RE::GetDurationOfApplicationRunTime());
					const auto doubleTapTicks = static_cast<std::uint64_t>(settings.DoubleTapDuration * 1000.0f);

					if (a_event->IsDown() && waitingSecondTap &&
						now - lastTapTime <= doubleTapTicks) {
						secondTap = true;
						waitingSecondTap = false;
						bStoppingSprint = false;
						return;
					}
					else if (a_event->IsDown() && playerCharacter->IsSprinting()) {
						bStoppingSprint = true;
						waitingSecondTap = false;
						secondTap = false;
					}
					else if (a_event->IsDown() && waitingSecondTap) {
						waitingSecondTap = false;
					}
					else if (secondTap) {
						if (a_event->IsUp()) {
							if (a_event->HeldDuration() < settings.SprintingPressDuration) {
								spdlog::info("TKRE::dodge");
								TKRE::dodge(false, true);
							}
							spdlog::info("secondTap: {}, but over SprintingPressDuration", secondTap);
							secondTap = false;
							bStoppingSprint = false;
							return;
						}

						if (a_event->IsHeld()) {
							if (a_event->HeldDuration() < settings.SprintingPressDuration) {
								spdlog::info("secondTap: {}, but under SprintingPressDuration", secondTap);
								return;
							}
							spdlog::info("secondTap: {}, but over SprintingPressDuration, return vanilla button event", secondTap);
							secondTap = false;
							a_event->GetRuntimeData().heldDownSecs = 0.f;
						}
					}
					else if (a_event->IsUp()) {
						if (a_event->HeldDuration() < settings.SprintingPressDuration && !bStoppingSprint) {
							lastTapTime = now;
							waitingSecondTap = true;
							spdlog::info("IsUp waitingSecondTap");
						}
						else {
							waitingSecondTap = false;
							spdlog::info("IsUp reset waitingSecondTap");
						}

						bStoppingSprint = false;
					}
				}
			}
			else {
				if (userEvent == userEvents->sprint) {
					if (a_event->IsDown() && playerCharacter->IsSprinting()) {
						bStoppingSprint = true;
					}
					else if (a_event->HeldDuration() < settings.SprintingPressDuration) {
						if (a_event->IsUp()) {
							TKRE::dodge();
							bStoppingSprint = false;
						}
						return;
					}
					else if (!playerCharacter->IsSprinting() && !bStoppingSprint) {
						a_event->heldDownSecs = 0.f;
					}
					else if (a_event->IsUp()) {
						bStoppingSprint = false;
					}
				}
			}
		}

		_ProcessButton(a_this, a_event, a_data);
	}
}
