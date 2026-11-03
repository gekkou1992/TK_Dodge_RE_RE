#pragma once
//#include <cstddef>

class TKRE
{
public:
	static bool GetDodgeEvent(std::string& a_event);
	static bool GetDodgeEventEx(std::string& a_event, const bool a_requireMovement, const bool a_allowSprinting);

	//a_requireMovement = false : Allow Dodging in Place
	//a_allowSprinting = true : Only pressed sprint key
	static void dodge(const bool a_requireMovement = true, const bool a_allowSprinting = false);

	static void applyDodgeCost();

private:
	TKRE() = default;

	~TKRE() = default;

	TKRE(const TKRE&) = delete;

	TKRE(TKRE&&) = delete;

	TKRE& operator=(const TKRE&) = delete;

	TKRE& operator=(TKRE&&) = delete;
};