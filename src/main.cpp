#include "LoadGame.h"
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/msvc_sink.h>
#include <spdlog/spdlog.h>
namespace
{
	bool InitializeLog()
	{
		auto path = SKSE::log::log_directory();
		if (!path) {
			//スカイリムプロセスごと止まるからやめとけ
			//SKSE::stl::report_and_fail("Failed to find standard logging directory"sv);
			return false;
		}

		*path /= fmt::format("{}.log"sv, SKSE::GetPluginName());

		std::vector<spdlog::sink_ptr> sinks{ std::make_shared<spdlog::sinks::msvc_sink_mt>(), std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true) };

		auto logger = std::make_shared<spdlog::logger>("HBodyUtil", sinks.begin(), sinks.end());

		const auto level = spdlog::level::info;
		logger->set_level(level);
		logger->flush_on(level);

		spdlog::set_default_logger(std::move(logger));
		spdlog::set_pattern("[%T.%e] [%=5t] [%L] %v");
		//spdlog::set_pattern("%g(%#): [%^%l%$] %v");
		return true;
	}
}

SKSEPluginLoad(const SKSE::LoadInterface* a_skse)
{
	SKSE::Init(a_skse, false);
	if (!InitializeLog()) {
		return false;
	}

	// do stuff
	auto g_message = SKSE::GetMessagingInterface();
	if (!g_message) {
		spdlog::error("Messaging Interface Not Found!");
		return false;
	}

	g_message->RegisterListener(TKDodge::EventCallback);

	return true;
}
