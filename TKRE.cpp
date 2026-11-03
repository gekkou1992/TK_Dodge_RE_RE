#include "TKRE.h"
#include "Menu.h"


inline RE::NiPoint2 Vec2Rotate(const RE::NiPoint2& vec, float angle)
{
	RE::NiPoint2 ret;
	ret.x = vec.x * cos(angle) - vec.y * sin(angle);
	ret.y = vec.x * sin(angle) + vec.y * cos(angle);
	return ret;
}

inline float Vec2Length(const RE::NiPoint2& vec)
{
	return std::sqrtf(vec.x * vec.x + vec.y * vec.y);
}

inline RE::NiPoint2 Vec2Normalize(RE::NiPoint2& vec)
{
	RE::NiPoint2 ret(0.f, 0.f);
	float vecLength = Vec2Length(vec);
	if (vecLength == 0) {
		return ret;
	}
	float invlen = 1.0f / vecLength;
	ret.x = vec.x * invlen;
	ret.y = vec.y * invlen;
	return ret;
}

inline float DotProduct(RE::NiPoint2& a, RE::NiPoint2& b)
{
	return a.x * b.x + a.y * b.y;
}

inline float CrossProduct(RE::NiPoint2& a, RE::NiPoint2& b)
{
	return a.x * b.y - a.y * b.x;
}

inline float GetAngle(RE::NiPoint2& a, RE::NiPoint2& b)
{
	return atan2(CrossProduct(a, b), DotProduct(a, b));
}

inline bool isJumping(RE::Actor* a_actor)
{
	bool result = false;
	return a_actor->GetGraphVariableBool("bInJumpState", result) && result;
}

bool TKRE::GetDodgeEvent(std::string& a_event)
{
	auto direction = Vec2Normalize(RE::PlayerControls::GetSingleton()->data.prevMoveVec);

	if (direction.x == 0.f && direction.y == 0.f) {		
		return false;
	}

	auto tdm = RE::TESForm::LookupByEditorID<RE::TESGlobal>("TDM_DirectionalMovement");
	if (tdm && tdm->value) {
		spdlog::debug("TDM Free Movement, Force to Forward Dodge!");
		a_event = "TKDodgeForward";
		return true;
	}

	RE::NiPoint2 forwardVector(0.f, 1.f);
	const float angle = GetAngle(direction, forwardVector);

	constexpr float PI8 = std::numbers::pi_v<float> / 8.0f;

	if (angle >= -2 * PI8 && angle < 2 * PI8)
		a_event = "TKDodgeForward";
	else if (angle >= 2 * PI8 && angle < 6 * PI8)
		a_event = "TKDodgeRight";
	else if (angle >= -6 * PI8 && angle < -2 * PI8)
		a_event = "TKDodgeLeft";
	else
		a_event = "TKDodgeBack";

	return true;
}
bool TKRE::GetDodgeEventEx(std::string& a_event, bool a_requireMovement, bool a_allowSprinting)
{
	auto idm = RE::BSInputDeviceManager::GetSingleton();
	auto cm = RE::ControlMap::GetSingleton();
	auto ue = RE::UserEvents::GetSingleton();

	if (!idm || !cm || !ue)
		return true;
	
	auto keyboard = idm->GetKeyboard();
	if (!idm->IsGamepadEnabled() && !keyboard)
		return true;

	constexpr auto deviceType = RE::INPUT_DEVICE::kKeyboard;

	auto kforward = cm->GetMappedKey(ue->forward, deviceType);
	auto kback = cm->GetMappedKey(ue->back, deviceType);
	auto kleft = cm->GetMappedKey(ue->strafeLeft, deviceType);
	auto kright = cm->GetMappedKey(ue->strafeRight, deviceType);

	const auto isPressed = [&](std::uint32_t key) {
		return keyboard && key < sizeof(keyboard->curState) && (keyboard->curState[key] & 0x80) != 0;
		};
	auto SetAnimGraph = [](const float direction) {
		auto player = RE::PlayerCharacter::GetSingleton();
		if (player && player->SetGraphVariableFloat("fTKDodgePCDirection", direction)) {
			spdlog::info("{} SetGraphVariableFloat to {:f}", player->GetName(), direction);
		}
		};
	float x = 0.0f;
	float y = 0.0f;
	bool gamepadMapped = false;

	if (idm->IsGamepadEnabled()) {
		const auto gamepad = idm->GetGamepad();
		const auto win32Gamepad = gamepad ? skyrim_cast<RE::BSWin32GamepadDevice*>(gamepad) : nullptr;

		if (win32Gamepad) {
			RE::ControlMap::UserEventMapping mapping{};

			if (cm->GetMappingFromEventName(
				ue->move,
				RE::UserEvents::INPUT_CONTEXT_ID::kGameplay,
				RE::INPUT_DEVICE::kGamepad,
				mapping)) {

				const auto& state = win32Gamepad->GetRuntimeData();
				constexpr float deadZone = 0.15f;

				if (mapping.inputKey == RE::BSWin32GamepadDevice::Key::kLeftStick) {
					x = state.currentLX;
					y = state.currentLY;
					gamepadMapped = true;
				}
				else if (mapping.inputKey == RE::BSWin32GamepadDevice::Key::kRightStick) {
					x = state.currentRX;
					y = state.currentRY;
					gamepadMapped = true;
				}
				if (x * x + y * y < deadZone * deadZone) {
					x = 0.0f;
					y = 0.0f;
				}
			}
		}
	}
	else {
		x = static_cast<float>(int(isPressed(kright)) - int(isPressed(kleft)));
		y = static_cast<float>(int(isPressed(kforward)) - int(isPressed(kback)));
	}

	if (idm->IsGamepadEnabled() && !gamepadMapped)
		return true;

	if (x == 0.0f && y == 0.0f && a_requireMovement) {
		return false;
	}

	static const float STAND = -1.0f;
	static const float FORWARD = 0.0f;
	static const float FORWARD_RIGHT = 0.125f;
	static const float RIGHT = 0.25f;
	static const float BACKWARD_RIGHT = 0.375f;
	static const float BACKWARD = 0.5f;
	static const float BACKWARD_LEFT = 0.625f;
	static const float LEFT = 0.75f;
	static const float FORWARD_LEFT = 0.875f;

	if (x == 0 && y == 0) {
		SetAnimGraph(STAND);
		return true;
	}

	auto tdm = RE::TESForm::LookupByEditorID<RE::TESGlobal>("TDM_DirectionalMovement");
	if (tdm && tdm->value) {
		SetAnimGraph(FORWARD);
		a_event = "TKDodgeForward";

		/*auto pc = RE::PlayerCharacter::GetSingleton();
		auto camera = RE::PlayerCamera::GetSingleton();

		if (pc && camera && camera->IsInThirdPerson()) {
			auto thirdPerson = static_cast<RE::ThirdPersonState*>(camera->currentState.get());						
			const float angle = std::remainder(thirdPerson->freeRotation.x * (180.0f / std::numbers::pi_v<float>), 360.0f);
			if (angle > -45.0f && angle < 45.0f) {
				//do nothing
				spdlog::info("{} Facing Forward", pc->GetName());
			}
			else if (angle >= 45.0f && angle < 135.0f) {
				spdlog::info("{} Facing Left", pc->GetName());
				std::tie(kforward, kright, kback, kleft) = std::make_tuple(kleft, kforward, kright, kback);
			}
			else if (angle >= 135.0f || angle <= -135.0f) {
				spdlog::info("{} Facing Backward", pc->GetName());
				std::swap(kforward, kback);
				std::swap(kleft, kright);
			}
			else {
				spdlog::info("{} Facing Right", pc->GetName());
				std::tie(kforward, kright, kback, kleft) = std::make_tuple(kright, kback, kleft, kforward);
			}
		}
		if (isPressed(kright))
			a_event = "TKDodgeRight";
		else if (isPressed(kleft))
			a_event = "TKDodgeLeft";
		else if (isPressed(kback))
			a_event = "TKDodgeBack";
		else if (isPressed(kforward))
			a_event = "TKDodgeForward";*/
	}
	else
	{
		const auto& settings = Settings::GetSingleton();
		//360未使用またはTDM不使用	
		const double degree = std::atan2(y, x) * (180.0 / std::numbers::pi);

		if (68.0 <= degree && 112.0 >= degree) {
			SetAnimGraph(FORWARD);
			a_event = "TKDodgeForward";
		}
		else if (68.0 > degree && 22.0 < degree) {
			SetAnimGraph(FORWARD_RIGHT);
			a_event = settings.PrioritizeHorizontalDodge ? "TKDodgeRight" : "TKDodgeForward";
		}
		else if (22.0 >= degree && -22.0 <= degree) {
			SetAnimGraph(RIGHT);
			a_event = "TKDodgeRight";
		}
		else if (-22.0 > degree && -68.0 < degree) {
			SetAnimGraph(BACKWARD_RIGHT);
			a_event = settings.PrioritizeHorizontalDodge ? "TKDodgeRight" : "TKDodgeBack";
		}
		else if (-68.0 >= degree && -112.0 <= degree) {
			SetAnimGraph(BACKWARD);
			a_event = "TKDodgeBack";
		}
		else if (-112.0 > degree && -158.0 < degree) {
			SetAnimGraph(BACKWARD_LEFT);
			a_event = settings.PrioritizeHorizontalDodge ? "TKDodgeLeft" : "TKDodgeBack";
		}
		else if (112.0 < degree && 158.0 > degree) {
			SetAnimGraph(FORWARD_LEFT);
			a_event = settings.PrioritizeHorizontalDodge ? "TKDodgeLeft" : "TKDodgeForward";
		}
		else if (-158.0 >= degree || 158.0 <= degree) {
			SetAnimGraph(LEFT);
			a_event = "TKDodgeLeft";
		}
	}

	return true;
}
//bunch of ugly checks
inline bool canDodge(RE::PlayerCharacter* a_pc, const bool a_allowSprinting)
{
	if (!a_pc)
		return false;

	auto playerControls = RE::PlayerControls::GetSingleton();
	auto controlMap = RE::ControlMap::GetSingleton();
	auto actorState = a_pc->AsActorState();
	auto actorValueOwner = a_pc->AsActorValueOwner();
	const auto& settings = Settings::GetSingleton();

	if (!playerControls || !controlMap || !actorState || !actorValueOwner)
		return false;

	// Animation
	bool bIsDodging = false;
	if (!a_pc->GetGraphVariableBool("bIsDodging", bIsDodging) || bIsDodging)
		return false;

	// Attack
	if (actorState->GetAttackState() != RE::ATTACK_STATE_ENUM::kNone &&
		!settings.enableDodgeAttackCancel)
		return false;

	// Sprint / Sneak
	if (actorState->IsSprinting() && settings.EnableSprintKeyDodge && !a_allowSprinting)
		return false;

	if (a_pc->IsSneaking() && !settings.enableSneakDodge)
		return false;

	// Controls
	if (!controlMap->IsMovementControlsEnabled() ||
		!controlMap->IsFightingControlsEnabled())
		return false;

	if (!playerControls->attackBlockHandler ||
		!playerControls->attackBlockHandler->inputEventHandlingEnabled)
		return false;

	if (!playerControls->movementHandler ||
		!playerControls->movementHandler->inputEventHandlingEnabled)
		return false;

	// Actor state
	if (actorState->GetSitSleepState() != RE::SIT_SLEEP_STATE::kNormal ||
		actorState->GetKnockState() != RE::KNOCK_STATE_ENUM::kNormal ||
		actorState->GetFlyState() != RE::FLY_STATE::kNone)
		return false;

	// Other restrictions
	if (a_pc->IsSwimming() || isJumping(a_pc) || a_pc->IsInKillMove())
		return false;

	// Stamina
	if (actorValueOwner->GetActorValue(RE::ActorValue::kStamina) < settings.dodgeStamina)
		return false;

	return true;
}

void TKRE::dodge(bool a_requireMovement, bool a_allowSprinting)
{
	spdlog::debug("dodging");
	auto pc = RE::PlayerCharacter::GetSingleton();
	if (!canDodge(pc, a_allowSprinting)) {
		spdlog::debug("cannot dodge");
		return;
	}
	const auto& settings = Settings::GetSingleton();
	std::string dodge_event = settings.defaultDodgeEvent;
	if (settings.EnableLegacyDodgeMethod)
	{
		if (!GetDodgeEvent(dodge_event))
			return;
	}
	else {
		if (!GetDodgeEventEx(dodge_event, a_requireMovement, a_allowSprinting))
			return;
	}

	//if (Settings::stepDodge) {
	//	pc->SetGraphVariableInt("iStep", 2);
	//}
	
	pc->SetGraphVariableInt("iStep", settings.stepDodge ? 2 : 0);
	
	pc->SetGraphVariableFloat("TKDR_IframeDuration", settings.iFrameDuration);  //Set invulnerable frame duration
	pc->NotifyAnimationGraph(dodge_event);                                       //Send TK Dodge Event
}

void TKRE::applyDodgeCost()
{
	auto pc = RE::PlayerCharacter::GetSingleton();
	if (!pc || pc->IsGodMode())
		return;

	auto actorValueOwner = pc->AsActorValueOwner();
	if (!actorValueOwner)
		return;

	actorValueOwner->DamageActorValue(RE::ActorValue::kStamina, - Settings::GetSingleton().dodgeStamina);
}