#pragma once
#include <SimpleIni.h>
#include "OAR_API/OpenAnimationReplacer-ConditionTypes.h"
#include "OAR_API/OpenAnimationReplacerAPI-Conditions.h"

inline constexpr std::array dodgeEvents{
"TKDodgeForward",
"TKDodgeBack",
"TKDodgeLeft",
"TKDodgeRight",
"EndAnimatedCamera"
};

class Settings
{
public:
	Settings() = default;
	~Settings() = default;
	Settings(const Settings&) = delete;
	Settings& operator=(const Settings&) = delete;
	Settings(Settings&&) = delete;
	Settings& operator=(Settings&&) = delete;
	static Settings& GetSingleton() {
		static Settings singleton;
		return singleton;
	}
	std::uint32_t DodgeKey{ 42 };
	bool EnableSprintKeyDodge{ false };
	bool EnableSneakKeyDodge{ false };
	bool StepDodge{ false };
	float DodgeStamina{ 10.f };
	bool EnableSneakDodge{ false };
	bool EnableDodgeAttackCancel{ true };
	float iFrameDuration{ 0.3f };
	std::string DefaultDodgeEvent{ "TKDodgeBack" };
	float SprintingPressDuration{ 0.3f };
	float SneakingPressDuration{ 0.3f };
	std::uint32_t GamepadDodgeKey{ UINT32_MAX };

	//add
	bool EnableLegacyDodgeMethod{ false };
	bool EnableDodgeFootStepSound{ true };
	std::atomic<bool> PrioritizeHorizontalDodge{ true };;
	bool EnableSprintKeyDoubleTap{ true };
	float DoubleTapDuration{ 0.3f };

	void readSettings(void);
	//add
	void readAdditionalSettings(void);
	void readCustomSettings(void);
	void writeCustomSettings();
private:
	static void ReadBoolSetting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, bool& a_setting);
	static void ReadFloatSetting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, float& a_setting);

	//add fixed
	static void ReadIntSetting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, int32_t& a_setting);
	static void ReadIntSetting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, uint32_t& a_setting);	
	static void WriteIntSetting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, const int32_t a_setting);
	static void WriteIntSetting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, const uint32_t a_setting);
	static void WriteFloatSetting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, const float a_setting);
	static void WriteStringSetting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, const std::string& a_setting);
	static void WriteBoolSetting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, const bool a_setting);
	
};

namespace tkdrere
{
	enum ButtonCodeOffset : uint32_t
	{
		kInvalid = UINT32_MAX,
		kKeyboardOffset = 0,
		kMouseOffset = 256,
		kGamepadOffset = 266
	};

	class LanguageManager
	{
	public:		
		LanguageManager(const LanguageManager&) = delete;
		LanguageManager& operator=(const LanguageManager&) = delete;
		LanguageManager(LanguageManager&&) = delete;
		LanguageManager& operator=(LanguageManager&&) = delete;

		struct GeneralLanguage
		{
			std::string sDodge{ "Dodge Settings" };
		};
		GeneralLanguage generelLanguage;

		struct DodgeLanguage
		{
			std::string DodgeHotkey{ "Dodge Key" };
			std::string DodgeHotkey_Description{ "Key used to dodge. Right-click to reset to default." };
			std::string EnableSprintKeyDodge{ "Enable Sprint Key Dodge" };
			std::string EnableSprintKeyDodge_Description{ "Enable dodging by tapping the sprint key. This replaces the dodge hotkey with the sprint key.\nRequires Classic Sprinting Redone(https://www.nexusmods.com/skyrimspecialedition/mods/20166) to work.\nRight-click to reset to default." };
			std::string StepDodge{ "Step Dodge" };
			std::string StepDodge_Description{ "Choose whether to perform a step dodge instead of a roll dodge.\n\"true\" = Enabled, \"false\" = Disabled.\nRight-click to reset to default." };
			std::string DodgeStamina{ "Dodge Stamina" };
			std::string DodgeStamina_Description{ "Set the stamina cost per dodge. Right-click to reset to default." };
			std::string EnableSneakDodge{ "Enable Sneak Dodge" };
			std::string EnableSneakDodge_Description{ "Allow dodging while sneaking. Right-click to reset to default." };
			std::string sBind{ "Bind" };
			std::string sBind_Description{ "Press any keyboard key or gamepad button to assign the dodge key.\nRight-click to reset to the default dodge key." };
			std::string sCancel{ "Cancel" };
			std::string sPressKeyOrButton{ "press a key or button (Escape cancels)" };
			std::string iFrameDuration{ "iFrame Duration" };
			std::string iFrameDuration_Description{ "Set the duration of invincibility frames in seconds.\nRight-click to reset to default." };
			std::string EnableDodgeAttackCancel{ "Enable Dodge Attack Cancel" };
			std::string EnableDodgeAttackCancel_Description{ "Allow dodging to cancel attacks.\nRight-click to reset to default." };
			std::string DefaultDodgeEvent{ "Default Dodge Event" };
			std::string DefaultDodgeEvent_Description{ "Default dodge animation played when the player is stationary.\nAvailable options: \"TKDodgeForward\", \"TKDodgeLeft\", \"TKDodgeRight\", \"TKDodgeBack\".\nLeaving it blank may cause a CTD.\nYou can also use \"EndAnimatedCamera\" to disable this feature.\nRight-click to reset to default." };
			std::string EnableLegacyDodgeMethod{ "Enable Legacy Dodge Method" };
			std::string EnableLegacyDodgeMethod_Description{ "false = Original TK Dodge RE behavior.\ntrue = Legacy dodge method.\nRight-click to reset to default." };
			std::string EnableDodgeFootStepSound{ "Enable Dodge Footstep Sound" };
			std::string EnableDodgeFootStepSound_Description{ "Enable footstep sounds during dodging. Requires footstep annotations in the dodge animation files.\nRight-click to reset to default." };
			std::string PrioritizeHorizontalDodge{ "Prioritize Horizontal Dodge" };
			std::string PrioritizeHorizontalDodge_Description{ "Prioritize horizontal or vertical dodge animations for diagonal input.\n1 = Horizontal(Left / Right), 0 = Vertical(Forward / Backward).\nRight-click to reset to default." };
			//std::string GamepadDodgeKey{ "Gamepad Dodge Key" };
			//std::string GamepadDodgeKey_Description{ "Press this button to perform a dodge.\nRight-click to reset to default." };
			std::string EnableSprintKeyDoubleTap{ "Enable Sprint Key Double Tap" };
			std::string EnableSprintKeyDoubleTap_Description{ "Enable dodging by double-tapping the sprint key.\nRight-click to reset to default." };
			std::string DoubleTapDuration{ "Double Tap Duration" };
			std::string DoubleTapDuration_Description{ "Maximum time between the first key release and the second key press (in seconds).\nRight-click to reset to default." };
		};
		DodgeLanguage dodgeLanguage;

		static LanguageManager& GetSingleton(void);
		void LoadLanguage(void);
	private:
		LanguageManager() = default;
		~LanguageManager() = default;
		void ReadStringSetting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, std::string& a_setting);
	};

	class PrioritizeHorizontalDodgeCondition : public Conditions::CustomCondition
	{
	public:
		constexpr static inline std::string_view CONDITION_NAME = "GetPrioritizeHorizontalTKDodgeRERE"sv;
		PrioritizeHorizontalDodgeCondition();
		RE::BSString GetName() const override { return CONDITION_NAME.data(); }
		RE::BSString GetDescription() const override { return "Checks whether \"TK Dodge RE RE\" horizontal dodge priority is enabled. For only the player character."sv.data(); }
		constexpr REL::Version GetRequiredVersion() const override { return { 1, 0, 0 }; }		
		RE::BSString GetCurrent(RE::TESObjectREFR* a_refr) const override;

	protected:
		bool EvaluateImpl(RE::TESObjectREFR* a_refr, RE::hkbClipGenerator* a_clipGenerator, void* a_subMod) const override;
	private:
		std::uint32_t screenWidth;
		std::uint32_t screenHeight;
	};

	void Register();
}