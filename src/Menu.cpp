#include "Menu.h"
#include "InputEvents.h"
#include "SKSEMenuFramework/SKSEMenuFramework.h"

void Settings::ReadIntSetting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, int32_t& a_setting)
{
	if (a_ini.GetValue(a_sectionName, a_settingName)) {
		a_setting = static_cast<int32_t>(a_ini.GetLongValue(a_sectionName, a_settingName));
	}
}
void Settings::ReadIntSetting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, uint32_t& a_setting)
{
	const char* value = a_ini.GetValue(a_sectionName, a_settingName);
	if (value) {
		try {
			a_setting = static_cast<uint32_t>(std::stoul(value, nullptr, 0));
		}
		catch (const std::exception&) {
			spdlog::warn("Invalid uint32 setting: {}", a_settingName);
		}
	}
}
void Settings::ReadFloatSetting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, float& a_setting)
{
	const char* bFound = nullptr;
	bFound = a_ini.GetValue(a_sectionName, a_settingName);
	if (bFound) {
		//INFO("found {} with value {}", a_settingName, bFound);
		a_setting = static_cast<float>(a_ini.GetDoubleValue(a_sectionName, a_settingName));
	}
}

void Settings::ReadBoolSetting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, bool& a_setting)
{
	const char* bFound = nullptr;
	bFound = a_ini.GetValue(a_sectionName, a_settingName);
	if (bFound) {
		//INFO("found {} with value {}", a_settingName, bFound);
		a_setting = a_ini.GetBoolValue(a_sectionName, a_settingName);
	}
}
void Settings::WriteIntSetting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, const int32_t a_setting)
{
	a_ini.SetLongValue(a_sectionName, a_settingName, a_setting);
}
void Settings::WriteIntSetting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, const uint32_t a_setting)
{
	const auto value = std::to_string(a_setting);
	a_ini.SetValue(a_sectionName, a_settingName, value.c_str());
}
void Settings::WriteFloatSetting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, const float a_setting)
{
	char buffer[50]{};
	auto begin = std::begin(buffer);
	auto end = std::end(buffer) - 1;
	auto [ptr, ec] = std::to_chars(begin, end, a_setting, std::chars_format::fixed, 2);

	if (ec == std::errc()) {
		*ptr = '\0';
		a_ini.SetValue(a_sectionName, a_settingName, buffer);
	}
	else {
		spdlog::error("Failed to write .ini float value: {}", a_settingName);
	}
}
void Settings::WriteStringSetting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, const std::string& a_setting)
{
	a_ini.SetValue(a_sectionName, a_settingName, a_setting.c_str());
}
void Settings::WriteBoolSetting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, const bool a_setting)
{
	a_ini.SetBoolValue(a_sectionName, a_settingName, a_setting);
}

void Settings::readSettings()
{
	spdlog::info("Reading Settings...");
	CSimpleIniA ini;
	if (ini.LoadFile("Data\\SKSE\\Plugins\\TK Dodge RE.ini") < 0){
		spdlog::error("Failed to load \"TK Dodge RE.ini\"");
		return;
	}

	ReadIntSetting(ini, "Main", "DodgeHotkey", DodgeKey);
	ReadBoolSetting(ini, "Main", "EnableSprintKeyDodge", EnableSprintKeyDodge);
	ReadBoolSetting(ini, "Main", "EnableSneakKeyDodge", EnableSneakKeyDodge);
	ReadBoolSetting(ini, "Main", "StepDodge", StepDodge);
	ReadBoolSetting(ini, "Main", "EnableDodgeAttackCancel", EnableDodgeAttackCancel);
	ReadFloatSetting(ini, "Main", "DodgeStamina", DodgeStamina);
	ReadBoolSetting(ini, "Main", "EnableSneakDodge", EnableSneakDodge);
	ReadFloatSetting(ini, "Main", "iFrameDuration", iFrameDuration);
	if (const auto value = ini.GetValue("Main", "DefaultDodgeEvent")) {
		const auto it = std::ranges::find(dodgeEvents, value);
		DefaultDodgeEvent = it != dodgeEvents.end() ? value : "TKDodgeBack";
	}
	ReadFloatSetting(ini, "Main", "SprintingPressDuration", SprintingPressDuration);
	if (iFrameDuration < 0.f) {
		iFrameDuration = 0.f;
	}

	if (ini.GetBoolValue("Main", "EnableDebugLog")) {
		spdlog::set_level(spdlog::level::debug);
		spdlog::debug("Debug log enabled");
	}
}
void Settings::readAdditionalSettings()
{
	const std::filesystem::path filePath = "Data/SKSE/Plugins/TK Dodge RE RE/TK Dodge RE RE.ini";
	CSimpleIniA ini;
	ini.SetUnicode();
	if (ini.LoadFile(filePath.c_str()) < 0) {
		spdlog::error("Failed to load \"TK Dodge RE RE.ini\"");
	}
	ReadBoolSetting(ini, "Main", "EnableLegacyDodgeMethod", EnableLegacyDodgeMethod);
	ReadBoolSetting(ini, "Main", "EnableDodgeFootStepSound", EnableDodgeFootStepSound);
	bool _PrioritizeHorizontalDodge = PrioritizeHorizontalDodge.load(std::memory_order_relaxed);
	ReadBoolSetting(ini, "Main", "PrioritizeHorizontalDodge", _PrioritizeHorizontalDodge);
	PrioritizeHorizontalDodge.store(_PrioritizeHorizontalDodge, std::memory_order_relaxed);
	ReadIntSetting(ini, "Main", "GamepadDodgeKey", GamepadDodgeKey);
	ReadBoolSetting(ini, "Main", "EnableSprintKeyDoubleTap", EnableSprintKeyDoubleTap);
	ReadFloatSetting(ini, "Main", "DoubleTapDuration", DoubleTapDuration);
	DoubleTapDuration = std::clamp(DoubleTapDuration, 0.1f, 0.5f);
	spdlog::info("...done");	
}
void Settings::readCustomSettings()
{
	spdlog::info("Override Settings...");
	CSimpleIniA ini;
	ini.SetUnicode();
	if (ini.LoadFile("Data/SKSE/Plugins/TK Dodge RE RE/Custom.ini") < 0) {
		//spdlog::error("Failed to load \"Custom.ini\"");
		return;
	}
	ReadIntSetting(ini, "Main", "DodgeHotkey", DodgeKey);
	ReadBoolSetting(ini, "Main", "EnableSprintKeyDodge", EnableSprintKeyDodge);
	ReadBoolSetting(ini, "Main", "EnableSneakKeyDodge", EnableSneakKeyDodge);
	ReadBoolSetting(ini, "Main", "StepDodge", StepDodge);
	ReadBoolSetting(ini, "Main", "EnableDodgeAttackCancel", EnableDodgeAttackCancel);
	ReadFloatSetting(ini, "Main", "DodgeStamina", DodgeStamina);
	ReadBoolSetting(ini, "Main", "EnableSneakDodge", EnableSneakDodge);
	ReadFloatSetting(ini, "Main", "iFrameDuration", iFrameDuration);
	if (const auto value = ini.GetValue("Main", "DefaultDodgeEvent")) {
		const auto it = std::ranges::find(dodgeEvents, value);
		DefaultDodgeEvent = it != dodgeEvents.end() ? value : "TKDodgeBack";
	}
	ReadFloatSetting(ini, "Main", "SprintingPressDuration", SprintingPressDuration);
	if (iFrameDuration < 0.f) {
		iFrameDuration = 0.f;
	}

	if (ini.GetBoolValue("Main", "EnableDebugLog")) {
		spdlog::set_level(spdlog::level::debug);
		spdlog::debug("Debug log enabled");
	}
	ReadBoolSetting(ini, "Main", "EnableLegacyDodgeMethod", EnableLegacyDodgeMethod);
	ReadBoolSetting(ini, "Main", "EnableDodgeFootStepSound", EnableDodgeFootStepSound);
	bool _PrioritizeHorizontalDodge = PrioritizeHorizontalDodge.load(std::memory_order_relaxed);
	ReadBoolSetting(ini, "Main", "PrioritizeHorizontalDodge", _PrioritizeHorizontalDodge);
	PrioritizeHorizontalDodge.store(_PrioritizeHorizontalDodge, std::memory_order_relaxed);
	ReadIntSetting(ini, "Main", "GamepadDodgeKey", GamepadDodgeKey);
	ReadBoolSetting(ini, "Main", "EnableSprintKeyDoubleTap", EnableSprintKeyDoubleTap);
	ReadFloatSetting(ini, "Main", "DoubleTapDuration", DoubleTapDuration);
	DoubleTapDuration = std::clamp(DoubleTapDuration, 0.1f, 0.5f);
	spdlog::info("...done");
}
void Settings::writeCustomSettings()
{
	const std::filesystem::path filePath = "Data/SKSE/Plugins/TK Dodge RE RE/Custom.ini";

	std::error_code ec;
	std::filesystem::create_directories(filePath.parent_path(), ec);
	if (ec) {
		spdlog::error("Failed to create directory: {} ({})", filePath.parent_path().string(), ec.message());
		return;
	}

	CSimpleIniA ini;
	ini.SetUnicode();

	WriteIntSetting(ini, "Main", "DodgeHotkey", DodgeKey);
	WriteBoolSetting(ini, "Main", "EnableSprintKeyDodge", EnableSprintKeyDodge);
	WriteBoolSetting(ini, "Main", "StepDodge", StepDodge);
	WriteFloatSetting(ini, "Main", "DodgeStamina", DodgeStamina);
	//WriteBoolSetting(ini, "Main", "EnableSneakKeyDodge", EnableSneakKeyDodge);	
	WriteBoolSetting(ini, "Main", "EnableSneakDodge", EnableSneakDodge);
	WriteFloatSetting(ini, "Main", "iFrameDuration", iFrameDuration);
	WriteBoolSetting(ini, "Main", "EnableDodgeAttackCancel", EnableDodgeAttackCancel);
	WriteStringSetting(ini, "Main", "DefaultDodgeEvent", DefaultDodgeEvent);
	WriteFloatSetting(ini, "Main", "SprintingPressDuration", SprintingPressDuration);
	WriteBoolSetting(ini, "Main", "EnableLegacyDodgeMethod", EnableLegacyDodgeMethod);
	WriteBoolSetting(ini, "Main", "EnableDodgeFootStepSound", EnableDodgeFootStepSound);
	WriteBoolSetting(ini, "Main", "PrioritizeHorizontalDodge", PrioritizeHorizontalDodge.load(std::memory_order_relaxed));
	//WriteIntSetting(ini, "Main", "GamepadDodgeKey", GamepadDodgeKey);
	WriteBoolSetting(ini, "Main", "EnableSprintKeyDoubleTap", EnableSprintKeyDoubleTap);
	WriteFloatSetting(ini, "Main", "DoubleTapDuration", DoubleTapDuration);

	if (ini.SaveFile(filePath.c_str()) != SI_OK) {
		spdlog::error("Failed to save Custom.ini");
		return;
	}
	spdlog::info("Succeeded to save {}", filePath.string());
}

static std::uint32_t editDodgeConfig = 0;
static SKSEMenuFramework::Model::InputEvent* g_inputEvent{ nullptr };
static std::unique_ptr<SKSEMenuFramework::Model::Event> menuEvent;
static float playerStaminaMax = 100.0f;

std::atomic<std::uint32_t> g_capturedKey{ 0 };
std::atomic<bool> g_captureTarget{ false };
struct KeyName
{
	std::int32_t code;
	const char* name;
};
constexpr KeyName kKeyNames[] = {
	{ 1, "Escape" }, { 2, "1" }, { 3, "2" }, { 4, "3" }, { 5, "4" }, { 6, "5" }, { 7, "6" }, { 8, "7" }, { 9, "8" }, { 10, "9" }, { 11, "0" },
	{ 12, "-" }, { 13, "=" }, { 14, "Backspace" }, { 15, "Tab" },
	{ 16, "Q" }, { 17, "W" }, { 18, "E" }, { 19, "R" }, { 20, "T" }, { 21, "Y" }, { 22, "U" }, { 23, "I" }, { 24, "O" }, { 25, "P" },
	{ 26, "[" }, { 27, "]" }, { 28, "Enter" }, { 29, "Left Ctrl" },
	{ 30, "A" }, { 31, "S" }, { 32, "D" }, { 33, "F" }, { 34, "G" }, { 35, "H" }, { 36, "J" }, { 37, "K" }, { 38, "L" },
	{ 39, ";" }, { 40, "'" }, { 41, "`" }, { 42, "Left Shift" }, { 43, "\\" },
	{ 44, "Z" }, { 45, "X" }, { 46, "C" }, { 47, "V" }, { 48, "B" }, { 49, "N" }, { 50, "M" }, { 51, "," }, { 52, "." }, { 53, "/" },
	{ 54, "Right Shift" }, { 55, "Numpad *" }, { 56, "Left Alt" }, { 57, "Space" }, { 58, "Caps Lock" },
	{ 59, "F1" }, { 60, "F2" }, { 61, "F3" }, { 62, "F4" }, { 63, "F5" }, { 64, "F6" }, { 65, "F7" }, { 66, "F8" }, { 67, "F9" }, { 68, "F10" },
	{ 69, "Num Lock" }, { 70, "Scroll Lock" }, { 71, "Numpad 7" }, { 72, "Numpad 8" }, { 73, "Numpad 9" }, { 74, "Numpad -" },
	{ 75, "Numpad 4" }, { 76, "Numpad 5" }, { 77, "Numpad 6" }, { 78, "Numpad +" }, { 79, "Numpad 1" }, { 80, "Numpad 2" }, { 81, "Numpad 3" },
	{ 82, "Numpad 0" }, { 83, "Numpad ." }, { 87, "F11" }, { 88, "F12" },
	{ 156, "Numpad Enter" }, { 157, "Right Ctrl" }, { 181, "Numpad /" }, { 184, "Right Alt" },
	{ 199, "Home" }, { 200, "Up" }, { 201, "Page Up" }, { 203, "Left" }, { 205, "Right" }, { 207, "End" }, { 208, "Down" },
	{ 209, "Page Down" }, { 210, "Insert" }, { 211, "Delete" },
};
constexpr const char* kGamepadNames[] = {
	"D-pad Up", "D-pad Down", "D-pad Left", "D-pad Right", "Start", "Back",
	"Left Stick", "Right Stick", "LB", "RB", "A", "B", "X", "Y", "LT", "RT"
};
constexpr const char* kMouseNames[] = {
	"Left Button", "Right Button", "Middle Button", "Button 4", "Button 5"
};
constexpr std::array<std::pair<std::uint32_t, std::int32_t>, 16> kGamepadCodes{
	{
		{ RE::BSWin32GamepadDevice::Key::kUp, tkdrere::ButtonCodeOffset::kGamepadOffset + 0 },
		{ RE::BSWin32GamepadDevice::Key::kDown, tkdrere::ButtonCodeOffset::kGamepadOffset + 1 },
		{ RE::BSWin32GamepadDevice::Key::kLeft, tkdrere::ButtonCodeOffset::kGamepadOffset + 2 },
		{ RE::BSWin32GamepadDevice::Key::kRight, tkdrere::ButtonCodeOffset::kGamepadOffset + 3 },
		{ RE::BSWin32GamepadDevice::Key::kStart, tkdrere::ButtonCodeOffset::kGamepadOffset + 4 },
		{ RE::BSWin32GamepadDevice::Key::kBack, tkdrere::ButtonCodeOffset::kGamepadOffset + 5 },
		{ RE::BSWin32GamepadDevice::Key::kLeftThumb, tkdrere::ButtonCodeOffset::kGamepadOffset + 6 },
		{ RE::BSWin32GamepadDevice::Key::kRightThumb, tkdrere::ButtonCodeOffset::kGamepadOffset + 7 },
		{ RE::BSWin32GamepadDevice::Key::kLeftShoulder, tkdrere::ButtonCodeOffset::kGamepadOffset + 8 },
		{ RE::BSWin32GamepadDevice::Key::kRightShoulder, tkdrere::ButtonCodeOffset::kGamepadOffset + 9 },
		{ RE::BSWin32GamepadDevice::Key::kA, tkdrere::ButtonCodeOffset::kGamepadOffset + 10 },
		{ RE::BSWin32GamepadDevice::Key::kB, tkdrere::ButtonCodeOffset::kGamepadOffset + 11 },
		{ RE::BSWin32GamepadDevice::Key::kX, tkdrere::ButtonCodeOffset::kGamepadOffset + 12 },
		{ RE::BSWin32GamepadDevice::Key::kY, tkdrere::ButtonCodeOffset::kGamepadOffset + 13 },
		{ RE::BSWin32GamepadDevice::Key::kLeftTrigger, tkdrere::ButtonCodeOffset::kGamepadOffset + 14 },
		{ RE::BSWin32GamepadDevice::Key::kRightTrigger, tkdrere::ButtonCodeOffset::kGamepadOffset + 15 },
	}
};
constexpr std::array<std::pair<std::uint32_t, std::int32_t>, 5> kMouseCodes{
	{
		{ RE::BSWin32MouseDevice::Key::kLeftButton, tkdrere::ButtonCodeOffset::kMouseOffset + 0 },
		{ RE::BSWin32MouseDevice::Key::kRightButton, tkdrere::ButtonCodeOffset::kMouseOffset + 1 },
		{ RE::BSWin32MouseDevice::Key::kMiddleButton, tkdrere::ButtonCodeOffset::kMouseOffset + 2 },
		{ RE::BSWin32MouseDevice::Key::kButton3, tkdrere::ButtonCodeOffset::kMouseOffset + 3 },
		{ RE::BSWin32MouseDevice::Key::kButton4, tkdrere::ButtonCodeOffset::kMouseOffset + 4 },
	}
};
std::string KeyNameFor(std::int32_t a_code)
{
	if (a_code == 0) {
		return "-";
	}
	const auto mouseIndex = a_code - tkdrere::ButtonCodeOffset::kMouseOffset;
	if (mouseIndex >= 0 && mouseIndex < static_cast<std::int32_t>(std::size(kMouseNames))) {
		return std::format("Mouse {}", kMouseNames[mouseIndex]);
	}
	const auto gamepadIndex = a_code - tkdrere::ButtonCodeOffset::kGamepadOffset;
	if (gamepadIndex >= 0 && gamepadIndex < static_cast<std::int32_t>(std::size(kGamepadNames))) {
		return std::format("Gamepad {}", kGamepadNames[gamepadIndex]);
	}
	for (const auto& entry : kKeyNames) {
		if (entry.code == a_code) {
			return entry.name;
		}
	}
	return std::format("scancode {}", a_code);
}
static bool __stdcall OnInput(RE::InputEvent* a_event)
{
	if (!g_captureTarget.load() || g_capturedKey.load() != 0) {
		return false;
	}

	if (!SKSEMenuFramework::IsAnyBlockingWindowOpened()) {
		g_captureTarget.store(false);
		g_capturedKey.store(0);
		return false;
	}

	if (!a_event || a_event->GetEventType() != RE::INPUT_EVENT_TYPE::kButton) {
		return false;
	}

	const auto button = a_event->AsButtonEvent();
	if (!button || !button->IsDown()) {
		return false;
	}

	const auto GamepadCode = [](std::uint32_t buttonMask) -> std::int32_t {
		for (const auto& [mask, code] : kGamepadCodes) {
			if (mask == buttonMask) {
				return code;
			}
		}
		return -1;
		};

	const auto MouseCode = [](std::uint32_t buttonID) -> std::int32_t {
		for (const auto& [mask, code] : kMouseCodes) {
			if (mask == buttonID) {
				return code;
			}
		}
		return -1;
		};

	switch (button->GetDevice()) {
	case RE::INPUT_DEVICE::kKeyboard:
		if (button->GetIDCode() == 0x01) {
			g_captureTarget.store(false);
			g_capturedKey.store(0);
			return true;
		}
		g_capturedKey.store(button->GetIDCode());
		break;

	case RE::INPUT_DEVICE::kGamepad:
		if (const auto code = GamepadCode(button->GetIDCode()); code >= 0) {
			g_capturedKey.store(code);
			break;
		}
		return false;
	case RE::INPUT_DEVICE::kMouse:
		if (const auto code = MouseCode(button->GetIDCode()); code >= 0) {
			g_capturedKey.store(code);
			break;
		}
		return false;
	default:
		return false;
	}

	return true;
}
void StartInputEvent()
{
	if (!g_inputEvent) {
		g_inputEvent = SKSEMenuFramework::AddInputEvent(OnInput);
	}
}
void StopInputEvent()
{
	delete g_inputEvent;
	g_inputEvent = nullptr;
}
static void __stdcall OnMenuEvent(SKSEMenuFramework::Model::EventType eventType)
{	
	if (eventType == SKSEMenuFramework::Model::kCloseMenu) {
		g_captureTarget.store(false);
		g_capturedKey.store(0);
		StopInputEvent();

		if (0 < editDodgeConfig) {
			Settings::GetSingleton().writeCustomSettings();
			editDodgeConfig = 0;
		}
	}
	else if (eventType == SKSEMenuFramework::Model::kOpenMenu) {
		auto player = RE::PlayerCharacter::GetSingleton();
		if (player) {
			playerStaminaMax = player->GetActorValueMax(RE::ActorValue::kStamina);
		}
	}
}

static bool RenderConfigCheckbox(const std::string& label, bool& value, const bool defaultValue, const std::string& description)
{
	bool changed = ImGuiMCP::Checkbox(label.c_str(), &value);

	if (ImGuiMCP::IsItemClicked(ImGuiMCP::ImGuiMouseButton_Right)) {
		if (value != defaultValue) {
			value = defaultValue;
			changed = true;
		}
	}

	if (ImGuiMCP::IsItemHovered(ImGuiMCP::ImGuiHoveredFlags_AllowWhenDisabled)) {
		ImGuiMCP::SetTooltip("%s", description.c_str());
	}

	return changed;
}
static bool RenderConfigCheckbox(const std::string& label, std::atomic<bool>& value, const std::atomic<bool>& defaultValue, const std::string& description)
{
	bool currentValue = value.load(std::memory_order_relaxed);

	if (RenderConfigCheckbox(label, currentValue, defaultValue.load(std::memory_order_relaxed), description)) {
		value.store(currentValue, std::memory_order_relaxed);
		return true;
	}

	return false;
}
static bool RenderConfigSliderFloat(const std::string& label, float& value, const float defaultValue, const float minValue, const float maxValue, const char* format, const std::string& description)
{
	ImGuiMCP::SliderFloat(label.c_str(), &value, minValue, maxValue, format, ImGuiMCP::ImGuiSliderFlags_AlwaysClamp);

	bool changed = ImGuiMCP::IsItemDeactivatedAfterEdit();

	if (ImGuiMCP::IsItemClicked(ImGuiMCP::ImGuiMouseButton_Right)) {
		if (value != defaultValue) {
			value = defaultValue;
			changed = true;
		}
	}

	if (ImGuiMCP::IsItemHovered()) {
		ImGuiMCP::SetTooltip("%s", description.c_str());
	}

	return changed;
}
static void RenderDodgeKeyBind(Settings& _settings, const Settings& defaultSettings)
{
	auto& dodging = tkdrere::LanguageManager::GetSingleton().dodgeLanguage;

	if (!g_captureTarget.load() && g_inputEvent) {
		StopInputEvent();
	}

	if (g_captureTarget.load()) {
		if (const auto captured = g_capturedKey.exchange(0); captured != 0) {
			_settings.DodgeKey = captured;
			++editDodgeConfig;
			g_captureTarget.store(false);
			StopInputEvent();
		}
	}

	const auto resetKey = [&]() {
		if (_settings.DodgeKey != defaultSettings.DodgeKey) {
			_settings.DodgeKey = defaultSettings.DodgeKey;
			g_capturedKey.store(0);
			++editDodgeConfig;
		}
		};

	ImGuiMCP::AlignTextToFramePadding();
	ImGuiMCP::Text("%s: { %s }",
		dodging.DodgeHotkey.c_str(),
		KeyNameFor(_settings.DodgeKey).c_str());

	if (ImGuiMCP::IsItemClicked(ImGuiMCP::ImGuiMouseButton_Right)) {
		resetKey();
	}

	if (ImGuiMCP::IsItemHovered()) {
		ImGuiMCP::SetTooltip("%s", dodging.DodgeHotkey_Description.c_str());
	}

	ImGuiMCP::SameLine();

	if (g_captureTarget.load()) {
		ImGuiMCP::Text("%s", dodging.sPressKeyOrButton.c_str());
		ImGuiMCP::SameLine();

		if (ImGuiMCP::SmallButton(dodging.sCancel.c_str())) {
			StopInputEvent();
			g_captureTarget.store(false);
			g_capturedKey.store(0);
		}
	}
	else {
		if (ImGuiMCP::Button(dodging.sBind.c_str())) {
			StartInputEvent();
			g_captureTarget.store(true);
			g_capturedKey.store(0);
		}

		if (ImGuiMCP::IsItemClicked(ImGuiMCP::ImGuiMouseButton_Right)) {
			resetKey();
		}

		ImGuiMCP::SetItemTooltip("%s", dodging.sBind_Description.c_str());
	}
}
static void RenderDefaultDodgeEvent(Settings& _settings, const Settings& defaultSettings)
{
	auto& dodging = tkdrere::LanguageManager::GetSingleton().dodgeLanguage;

	const auto it = std::ranges::find(dodgeEvents, _settings.DefaultDodgeEvent);
	int selectedEvent = it != dodgeEvents.end() ? static_cast<int>(it - dodgeEvents.begin()) : 0;

	if (ImGuiMCP::Combo(dodging.DefaultDodgeEvent.c_str(), &selectedEvent, dodgeEvents.data(), static_cast<int>(dodgeEvents.size()))) {
		_settings.DefaultDodgeEvent = dodgeEvents[selectedEvent];
		++editDodgeConfig;
	}

	if (ImGuiMCP::IsItemClicked(ImGuiMCP::ImGuiMouseButton_Right)) {
		if (_settings.DefaultDodgeEvent != defaultSettings.DefaultDodgeEvent) {
			_settings.DefaultDodgeEvent = defaultSettings.DefaultDodgeEvent;
			++editDodgeConfig;
		}
	}

	if (ImGuiMCP::IsItemHovered()) {
		ImGuiMCP::SetTooltip("%s", dodging.DefaultDodgeEvent_Description.c_str());
	}
}
void __stdcall RenderDodge()
{
	static const Settings defaultSettings{};
	static auto& _settings = Settings::GetSingleton();
	static auto& dodging = tkdrere::LanguageManager::GetSingleton().dodgeLanguage;

	RenderDodgeKeyBind(_settings, defaultSettings);

	editDodgeConfig += RenderConfigCheckbox(dodging.EnableSprintKeyDodge, _settings.EnableSprintKeyDodge, defaultSettings.EnableSprintKeyDodge, dodging.EnableSprintKeyDodge_Description);

	editDodgeConfig += RenderConfigCheckbox(dodging.StepDodge, _settings.StepDodge, defaultSettings.StepDodge, dodging.StepDodge_Description);

	editDodgeConfig += RenderConfigSliderFloat(dodging.DodgeStamina, _settings.DodgeStamina, defaultSettings.DodgeStamina, 0.0f, playerStaminaMax, "%.0f", dodging.DodgeStamina_Description);

	editDodgeConfig += RenderConfigCheckbox(dodging.EnableSneakDodge, _settings.EnableSneakDodge, defaultSettings.EnableSneakDodge, dodging.EnableSneakDodge_Description);

	editDodgeConfig += RenderConfigSliderFloat(dodging.iFrameDuration, _settings.iFrameDuration, defaultSettings.iFrameDuration, 0.0f, 1.0f, "%.1f", dodging.iFrameDuration_Description);

	editDodgeConfig += RenderConfigCheckbox(dodging.EnableDodgeAttackCancel, _settings.EnableDodgeAttackCancel, defaultSettings.EnableDodgeAttackCancel, dodging.EnableDodgeAttackCancel_Description);

	RenderDefaultDodgeEvent(_settings, defaultSettings);

	editDodgeConfig += RenderConfigCheckbox(dodging.EnableLegacyDodgeMethod, _settings.EnableLegacyDodgeMethod, defaultSettings.EnableLegacyDodgeMethod, dodging.EnableLegacyDodgeMethod_Description);

	editDodgeConfig += RenderConfigCheckbox(dodging.EnableDodgeFootStepSound, _settings.EnableDodgeFootStepSound, defaultSettings.EnableDodgeFootStepSound, dodging.EnableDodgeFootStepSound_Description);

	editDodgeConfig += RenderConfigCheckbox(dodging.PrioritizeHorizontalDodge, _settings.PrioritizeHorizontalDodge, defaultSettings.PrioritizeHorizontalDodge, dodging.PrioritizeHorizontalDodge_Description);

	editDodgeConfig += RenderConfigCheckbox(dodging.EnableSprintKeyDoubleTap, _settings.EnableSprintKeyDoubleTap, defaultSettings.EnableSprintKeyDoubleTap, dodging.EnableSprintKeyDoubleTap_Description);

	editDodgeConfig += RenderConfigSliderFloat(dodging.DoubleTapDuration, _settings.DoubleTapDuration, defaultSettings.DoubleTapDuration, 0.1f, 0.5f, "%.2f", dodging.DoubleTapDuration_Description);
}

namespace tkdrere
{	
	LanguageManager& LanguageManager::GetSingleton()
	{
		static LanguageManager singleton;
		return singleton;
	}
	void LanguageManager::LoadLanguage()
	{		
		const std::filesystem::path filePath = "Data/SKSE/Plugins/TK Dodge RE RE/Language.ini";
		CSimpleIniA ini;
		ini.SetUnicode();
		if (ini.LoadFile(filePath.c_str()) < 0) {
			return;
		}
		ReadStringSetting(ini, "Main", "sDodge", generelLanguage.sDodge);
		ReadStringSetting(ini, "Main", "DodgeHotkey", dodgeLanguage.DodgeHotkey);
		ReadStringSetting(ini, "Main", "DodgeHotkey_Description", dodgeLanguage.DodgeHotkey_Description);
		ReadStringSetting(ini, "Main", "EnableSprintKeyDodge", dodgeLanguage.EnableSprintKeyDodge);
		ReadStringSetting(ini, "Main", "EnableSprintKeyDodge_Description", dodgeLanguage.EnableSprintKeyDodge_Description);
		ReadStringSetting(ini, "Main", "StepDodge", dodgeLanguage.StepDodge);
		ReadStringSetting(ini, "Main", "StepDodge_Description", dodgeLanguage.StepDodge_Description);
		ReadStringSetting(ini, "Main", "DodgeStamina", dodgeLanguage.DodgeStamina);
		ReadStringSetting(ini, "Main", "DodgeStamina_Description", dodgeLanguage.DodgeStamina_Description);
		ReadStringSetting(ini, "Main", "EnableSneakDodge", dodgeLanguage.EnableSneakDodge);
		ReadStringSetting(ini, "Main", "EnableSneakDodge_Description", dodgeLanguage.EnableSneakDodge_Description);
		ReadStringSetting(ini, "Main", "sBind", dodgeLanguage.sBind);
		ReadStringSetting(ini, "Main", "sBind_Description", dodgeLanguage.sBind_Description);
		ReadStringSetting(ini, "Main", "sCancel", dodgeLanguage.sCancel);
		ReadStringSetting(ini, "Main", "sPressKeyOrButton", dodgeLanguage.sPressKeyOrButton);
		ReadStringSetting(ini, "Main", "iFrameDuration", dodgeLanguage.iFrameDuration);
		ReadStringSetting(ini, "Main", "iFrameDuration_Description", dodgeLanguage.iFrameDuration_Description);
		ReadStringSetting(ini, "Main", "EnableDodgeAttackCancel", dodgeLanguage.EnableDodgeAttackCancel);
		ReadStringSetting(ini, "Main", "EnableDodgeAttackCancel_Description", dodgeLanguage.EnableDodgeAttackCancel_Description);
		ReadStringSetting(ini, "Main", "DefaultDodgeEvent", dodgeLanguage.DefaultDodgeEvent);
		ReadStringSetting(ini, "Main", "DefaultDodgeEvent_Description", dodgeLanguage.DefaultDodgeEvent_Description);
		ReadStringSetting(ini, "Main", "EnableLegacyDodgeMethod", dodgeLanguage.EnableLegacyDodgeMethod);
		ReadStringSetting(ini, "Main", "EnableLegacyDodgeMethod_Description", dodgeLanguage.EnableLegacyDodgeMethod_Description);
		ReadStringSetting(ini, "Main", "EnableDodgeFootStepSound", dodgeLanguage.EnableDodgeFootStepSound);
		ReadStringSetting(ini, "Main", "EnableDodgeFootStepSound_Description", dodgeLanguage.EnableDodgeFootStepSound_Description);
		ReadStringSetting(ini, "Main", "PrioritizeHorizontalDodge", dodgeLanguage.PrioritizeHorizontalDodge);
		ReadStringSetting(ini, "Main", "PrioritizeHorizontalDodge_Description", dodgeLanguage.PrioritizeHorizontalDodge_Description);
		//ReadStringSetting(ini, "Main", "GamepadDodgeKey", dodgeLanguage.GamepadDodgeKey);
		//ReadStringSetting(ini, "Main", "GamepadDodgeKey_Description", dodgeLanguage.GamepadDodgeKey_Description);
		ReadStringSetting(ini, "Main", "EnableSprintKeyDoubleTap", dodgeLanguage.EnableSprintKeyDoubleTap);
		ReadStringSetting(ini, "Main", "EnableSprintKeyDoubleTap_Description", dodgeLanguage.EnableSprintKeyDoubleTap_Description);
		ReadStringSetting(ini, "Main", "DoubleTapDuration", dodgeLanguage.DoubleTapDuration);
		ReadStringSetting(ini, "Main", "DoubleTapDuration_Description", dodgeLanguage.DoubleTapDuration_Description);
	}
	void LanguageManager::ReadStringSetting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, std::string& a_setting)
	{
		const char* value = a_ini.GetValue(a_sectionName, a_settingName);
		if (value) {
			a_setting = value;
		}
	}

	PrioritizeHorizontalDodgeCondition::PrioritizeHorizontalDodgeCondition() :
		screenWidth(1280), screenHeight(720)
	{		
		auto graphicsState = RE::BSGraphics::State::GetSingleton();
		if (graphicsState) {
			// 実際の描画解像度（横・縦）
			screenWidth = graphicsState->screenWidth;
			screenHeight = graphicsState->screenHeight;
		}
		//spdlog::info("screenWidth : {:d}, screenHeight : {:d}"sv, screenWidth, screenHeight);
	}	
	RE::BSString PrioritizeHorizontalDodgeCondition::GetCurrent(RE::TESObjectREFR* a_refr) const
	{
		return Settings::GetSingleton().PrioritizeHorizontalDodge.load(std::memory_order_relaxed) ? "true" : "false";
	}
	bool PrioritizeHorizontalDodgeCondition::EvaluateImpl([[maybe_unused]] RE::TESObjectREFR* a_refr, [[maybe_unused]] RE::hkbClipGenerator* a_clipGenerator, [[maybe_unused]] void* a_subMod) const
	{
		return Settings::GetSingleton().PrioritizeHorizontalDodge.load(std::memory_order_relaxed);
	}

	void Register()
	{
		if (!SKSEMenuFramework::IsInstalled()) {
			return;
		}
		auto& settings = Settings::GetSingleton();
		settings.readSettings();
		settings.readAdditionalSettings();
		settings.readCustomSettings();
		SKSEMenuFramework::SetSection("TK Dodge RE RE");
		auto& language = LanguageManager::GetSingleton();
		LanguageManager::GetSingleton().LoadLanguage();
		SKSEMenuFramework::AddSectionItem(language.generelLanguage.sDodge, RenderDodge);
		menuEvent.reset(SKSEMenuFramework::AddEvent(OnMenuEvent, 0.0f));
	}
}