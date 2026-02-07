#pragma once

#include <iostream>
#include <string_view>

namespace Logging {
	inline void Log(std::string_view text) {
		std::cout << "INFO: " << text << "\n";
	}

	inline void LogSilent(std::string_view text) {
		std::cout << text << "\n";
	}

	inline void Warn(std::string_view text) {
		std::cout << "WARNING: " << text << "\n";
	}

	inline void Error(std::string_view text) {
		std::clog << "ERROR: " << text << "\n";
	}
}