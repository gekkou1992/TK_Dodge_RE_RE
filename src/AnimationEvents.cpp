#include "AnimationEvents.h"
#include "TKRE.h"
#include "Menu.h"


namespace
{
	enum class DODGE_FOOTSTEP_TYPE : std::uint32_t
	{
		NONE = 0x00,
		JUMP_DOWN,
		JUMP_UP,
		SPRINT_R,
		SPRINT_L,
	};
	enum FIXED_ACTOR_MOVEMENT_TYPE : std::uint32_t
	{
		kWalk = 0,
		kRun = 1,
		kSprint = 2,
		kSneak = 3,
		kSwim = 4,

		kTotal = 5
	};

	bool IsInDodge(RE::Actor* act)
	{
		bool bIsDodging = false;
		if (act && act->GetGraphVariableBool("bIsDodging", bIsDodging) && bIsDodging)
			return bIsDodging;
		return bIsDodging;
	}
	void CreateDetectionEvent(RE::Actor* act, const RE::SOUND_LEVEL soundLevel)
	{
		if (!act)
			return;
		const auto& runtimeData = act->GetActorRuntimeData();
		if (runtimeData.currentProcess) {
			const auto level = RE::AIFormulas::GetSoundLevelValue(soundLevel);
			runtimeData.currentProcess->SetActorsDetectionEvent(act, act->data.location, level, act);
			
		}		
	}
	RE::BGSMaterialType* GetActorUnderMaterialFromCharacterControl(RE::Actor* act)
	{
		if (act) {
			auto cell = act->GetParentCell();
			auto ctrl = act->GetCharController();
			if (cell && ctrl) {
				if (ctrl->context.currentState == RE::hkpCharacterStateTypes::kOnGround) {
					if (act->IsInWater()) {
						return RE::BGSMaterialType::GetMaterialType(RE::MATERIAL_ID::kWater);
					}
					if (RE::MATERIAL_ID::kNone != ctrl->surfaceMaterial) {
						return RE::BGSMaterialType::GetMaterialType(ctrl->surfaceMaterial);
					}
					if (cell->GetRuntimeData().cellLand) {
						auto land = cell->GetRuntimeData().cellLand;
						if (land && land->loadedData) {
							// 1. セルの原点座標（世界座標系）を計算
							float cellX = 0.0f;
							float cellY = 0.0f;
							if (cell->GetCoordinates()) {
								cellX = cell->GetCoordinates()->cellX * 4096.0f;
								cellY = cell->GetCoordinates()->cellY * 4096.0f;
							}

							// 2. プレイヤーの世界座標を取得
							RE::NiPoint3 playerPos = act->GetPosition();

							// 3. セル内のローカル相対座標に変換
							float localX = playerPos.x - cellX;
							float localY = playerPos.y - cellY;

							// 3. どのクアッド（0:南西, 1:南東, 2:北西, 3:北東）にいるかを判定
							// セルの中心（2048）を基準に分ける
							int quadIndex = 0;
							if (localX >= 2048.0f && localY < 2048.0f)
								quadIndex = 1;  // 南東
							if (localX < 2048.0f && localY >= 2048.0f)
								quadIndex = 2;  // 北西
							if (localX >= 2048.0f && localY >= 2048.0f)
								quadIndex = 3;  // 北東

							RE::TESLandTexture* targetTexture = nullptr;
							if (land->loadedData->quadTextures[quadIndex][0]) {
								targetTexture = land->loadedData->quadTextures[quadIndex][0];
							}
							else if (land->loadedData->defQuadTextures[quadIndex]) {
								// ブレンドがなければデフォルトのクアッドテクスチャを使用
								targetTexture = land->loadedData->defQuadTextures[quadIndex];
							}

							// 6. テクスチャからマテリアルIDを引き抜く
							if (targetTexture && targetTexture->materialType) {
								//spdlog::info("From Cell position -> materialID : {:d}", static_cast<std::uint32_t>(targetTexture->materialType->materialID));
								return targetTexture->materialType;
							}
						}
					}
				}
			}
		}
		return nullptr;
	}
	void SetImpactEffect(const DODGE_FOOTSTEP_TYPE type, RE::Actor* act)
	{
		if (act && act->Is3DLoaded() && IsInDodge(act)) {
			std::string nodeName;
			std::string footStepName;
			switch (type) {
			case DODGE_FOOTSTEP_TYPE::JUMP_DOWN:
				footStepName = "jumpDown";
				nodeName = "NPC R Foot [Rft ]";
				break;
			case DODGE_FOOTSTEP_TYPE::JUMP_UP:
				footStepName = "JumpUp";
				nodeName = "NPC R Foot [Rft ]";
				break;
			case DODGE_FOOTSTEP_TYPE::SPRINT_R:
				footStepName = "FootSprintRight";
				nodeName = "NPC R Foot [Rft ]";
				break;
			case DODGE_FOOTSTEP_TYPE::SPRINT_L:
				footStepName = "FootSprintLeft";
				nodeName = "NPC L Foot [Lft ]";
				break;
			default:
				return;
			}
			if (nodeName.empty() || footStepName.empty())
				return;
		
			auto boots = act->GetWornArmor(RE::BGSBipedObjectForm::BipedObjectSlot::kFeet);
			if (!boots)
				boots = act->GetSkin(RE::BGSBipedObjectForm::BipedObjectSlot::kFeet);
			if (boots) {
				auto bootsAddon = boots->GetArmorAddonByMask(act->GetRace(), RE::BGSBipedObjectForm::BipedObjectSlot::kFeet);
				auto Set = bootsAddon ? bootsAddon->footstepSet : nullptr;
				if (Set) {
					auto movement = FIXED_ACTOR_MOVEMENT_TYPE::kSprint;
					if (Set->entries[FIXED_ACTOR_MOVEMENT_TYPE::kSprint].empty() && !Set->entries[FIXED_ACTOR_MOVEMENT_TYPE::kRun].empty())
						movement = FIXED_ACTOR_MOVEMENT_TYPE::kRun;
					for (auto footStep : Set->entries[movement]) {
						if (footStep && 0 == _stricmp(footStep->tag.c_str(), footStepName.c_str()) && footStep->impactSet) {

							auto material = GetActorUnderMaterialFromCharacterControl(act);
							if (material) {
								auto it = footStep->impactSet->impactMap.find(material);
								if (it != footStep->impactSet->impactMap.end() && it->second && it->second->sound1) {
									auto avo = act->Get3D1(false);
									auto manager = RE::BGSImpactManager::GetSingleton();
									if (manager && avo) {
										RE::BGSImpactManager::ImpactSoundData sound;
										sound.impactData = it->second;
										sound.position = &act->data.location;
										sound.objectToFollow = avo;
										sound.sound1 = nullptr;
										sound.sound2 = nullptr;
										sound.playSound1 = true;
										sound.playSound2 = true;
										sound.lowPriority = false;
										sound.pool = nullptr;

										[[maybe_unused]] bool bSucces = false;
										bSucces = manager->PlayImpactDataSounds(sound);
										if (bSucces && RE::SOUND_LEVEL::kSilent != it->second->data.soundLevel) {
											CreateDetectionEvent(act, it->second->data.soundLevel);
										}
										//spdlog::info("PlayImpactDataSounds : {}", bSucces);
									}
								}
							}
							return;
						}
					}
				}
			}

		}
	}	
}
animEventHandler::animEventHandler() : toggleEvents(0) {}
animEventHandler& animEventHandler::GetSingleton()
{
	static animEventHandler singleton;
	return singleton;
}
void animEventHandler::Register()
{
	EventControl(ANIMGRAPH_ADD);
}
void animEventHandler::EventControl(const std::uint32_t index)
{
	using EVENT = animEventHandler::EVENT_INDEX;
	switch (index) {
	case EVENT::ANIMGRAPH_ADD:
		if (0 == (toggleEvents & EVENT::ANIMGRAPH_ADD)) {
			auto player = RE::PlayerCharacter::GetSingleton();
			if (player && player->AddAnimationGraphEventSink(this)) {
				toggleEvents |= EVENT::ANIMGRAPH_ADD;
			}
		}
		break;
	case EVENT::ANIMGRAPH_REMOVE:
		if (0 != (toggleEvents & EVENT::ANIMGRAPH_ADD)) {
			auto player = RE::PlayerCharacter::GetSingleton();
			if (player) {
				RE::BSAnimationGraphManagerPtr graphManager;
				if (player->GetAnimationGraphManager(graphManager) && graphManager) {
					bool bSinked = false;
					for (auto& graph : graphManager->graphs) {
						if (bSinked)
							break;
						auto sorce = graph->GetEventSource<RE::BSAnimationGraphEvent>();
						if (sorce) {
							for (auto sink : sorce->sinks) {
								if (sink && sink == this) {
									bSinked = true;
									toggleEvents &= ~EVENT::ANIMGRAPH_ADD;
									sorce->RemoveEventSink(this);
									//spdlog::info("AnimationGraphEvent RemoveEventSink");
									break;
								}
							}
						}
					}
				}
			}
		}
		break;
	}
}
RE::BSEventNotifyControl animEventHandler::ProcessEvent(const RE::BSAnimationGraphEvent* a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>*)
{
	if (!a_event || !a_event->holder)
		return RE::BSEventNotifyControl::kContinue;

	if (a_event->tag == "TKDR_DodgeStart") {
		TKRE::applyDodgeCost();
	}
	else if (Settings::GetSingleton().EnableDodgeFootStepSound && 0 == _stricmp(a_event->tag.c_str(), "pie")) {
		//spdlog::info("Name : {}, PayLoad : {}, Tag : {}", a_event->holder->GetName(), a_event->payload.c_str(), a_event->tag.c_str());
		auto act = a_event->holder->As<RE::Actor>();
		if (act) {
			//spdlog::info("payload : {}", a_event->payload.c_str());
			if (a_event->payload == "TKDDodgeJumpUp") {
				SetImpactEffect(DODGE_FOOTSTEP_TYPE::JUMP_UP, act);
			}
			else if (a_event->payload == "TKDDodgeJumpDown") {
				SetImpactEffect(DODGE_FOOTSTEP_TYPE::JUMP_DOWN, act);
			}
			else if (a_event->payload == "TKDDodgeStepR") {				
				SetImpactEffect(DODGE_FOOTSTEP_TYPE::SPRINT_R, act);
			}
			else if (a_event->payload == "TKDDodgeStepL") {				
				SetImpactEffect(DODGE_FOOTSTEP_TYPE::SPRINT_L, act);
			}			
		}
	}
	return RE::BSEventNotifyControl::kContinue;
}
	
