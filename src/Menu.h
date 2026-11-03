#pragma once
#include <SimpleIni.h>

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
	std::uint32_t dodgeKey{ 42 };
	bool EnableSprintKeyDodge{ false };
	bool EnableSneakKeyDodge{ false };
	bool stepDodge{ false };
	float dodgeStamina{ 10.f };
	bool enableSneakDodge{ false };
	bool enableDodgeAttackCancel{ true };
	float iFrameDuration{ 0.3f };
	std::string defaultDodgeEvent{ "TKDodgeBack" };
	float SprintingPressDuration{ 0.3f };
	float SneakingPressDuration{ 0.3f };
	std::uint32_t GamepadDodgeKey{ 0xFFFFFFFF };

	//add
	bool EnableLegacyDodgeMethod{ false };
	bool EnableDodgeFootStepSound{ true };
	bool PrioritizeHorizontalDodge{ true };
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
	class LanguageManager
	{
	public:
		LanguageManager() = default;
		struct GeneralLanguage
		{
			//std::string sLanguageError_Title{ "Error" };
			//std::string sLanguageLoadError_Message{ "Failed to load the language file!" };
			//std::string sLanguageSaveError_Message{ "Failed to save the language file!" };
			//std::string sLanguage{ "Language" };
			//std::string sLanguage_Description{ "Select the language file to use." };
			//std::string sGeneral{ "General" };
			std::string sDodge{ "Dodge Settings" };			
			//std::string iLogLevel{ "Loglevel" };
			//std::string iLogLevel_Description{ "Controls the level of log output. Default is [2]." };

		};
		GeneralLanguage generelLanguage;

		struct DodgeLanguage
		{
			std::string DodgeHotkey{ "Dodge Key" };
			std::string DodgeHotkey_Description{ "Key used to perform a dodge." };
			std::string EnableSprintKeyDodge{ "Enable Sprint Key Dodge" };
			std::string EnableSprintKeyDodge_Description{ "Set the dodge perform by tapping sprint button, that will replace the dodge hotkey with sprint key.\n Require Classic Sprinting Redone(https://www.nexusmods.com/skyrimspecialedition/mods/20166) to work." };
			std::string	StepDodge{ "Step Dodge" };
			std::string	StepDodge_Description{ "Set if perform step dodge instead of rolling dodge, \"true\" is enable while \"false\" is disable." };
			std::string	DodgeStamina{ "Dodge Stamina" };
			std::string DodgeStamina_Description{ "Set the stamina cost when perform dodge." };
			std::string enableSneakDodge{ "Enable Sneak Dodge" };
			std::string enableSneakDodge_Description{ "Set if allow perfrom dodge when sneaking." };
			std::string sBind{ "Bind" };
			std::string sBind_Description{ "Press a keyboard key or gamepad button to assign the dodge key." };
			std::string sCancel{ "Cancel" };
			std::string sPressKeyOrButton{ "press a key or button (Escape cancels)" };
			std::string iFrameDuration{ "iFrame Duration" };
			std::string iFrameDuration_Description{ "Set the duration of invincibility frames. unit : second(s)." };
			std::string EnableDodgeAttackCancel{ "Enable Dodge Attack Cancel" };
			std::string EnableDodgeAttackCancel_Description{ "Allow dodge to cancel attacks." };
			std::string defaultDodgeEvent{ "default Dodge Event" };
			std::string defaultDodgeEvent_Description{ "default dodge event triggered when not moving.\n Available options: \"TKDodgeForward\", \"TKDodgeLeft\", \"TKDodgeRight\", \"TKDodgeBack\".\n Levae it blank may cause CTD,use \"EndAnimatedCamera\" can disable this feature too" };
			std::string EnableLegacyDodgeMethod{ "Enable Legacy Dodge Method" };
			std::string EnableLegacyDodgeMethod_Description{ "false = Original TK Dodge RE behavior.\ntrue = Legacy dodge method" };
			std::string EnableDodgeFootStepSound{ "Enable Dodge Foot Step Sound" };
			std::string EnableDodgeFootStepSound_Description{ "Add annotations to your favorite dodge .hkx files." };
			std::string PrioritizeHorizontalDodge{ "Prioritize Horizontal Dodge" };
			std::string PrioritizeHorizontalDodge_Description{ "Prioritize horizontal or vertical dodge animations for diagonal input.\n1 = Horizontal(Left / Right), 0 = Vertical(Forward / Backward)." };
			//std::string GamepadDodgeKey{ "Gamepad Dodge Key" };
			//std::string GamepadDodgeKey_Description{ "Gamepad Key used to perform a dodge." };
			std::string EnableSprintKeyDoubleTap{ "Enable Sprint Key Double Tap" };
			std::string EnableSprintKeyDoubleTap_Description{ "Enable dodging by double-tapping the sprint key." };
			std::string DoubleTapDuration{ "Double Tap Duration" };
			std::string DoubleTapDuration_Description{ "Maximum time between the first key release and the second key press (in seconds)." };
		};
		DodgeLanguage dodgeLanguage;
		static LanguageManager& GetSingleton(void);
		void LoadLanguage(void);
	private:
		void ReadStringSetting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, std::string& a_setting);
	};

	void Register();
}
