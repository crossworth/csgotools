#ifndef CSGOTOOLS_UTIL_HPP_
#define CSGOTOOLS_UTIL_HPP_

#include <iostream>
#include <string>
#include <chrono>

namespace csgotools {
	
	// TODO(Pedro) : fix this
	static std::string	GetLastPartFromPathString(const std::string& path) {
		std::string last_part = path;

		std::size_t last_dir_separator = 0;
		last_dir_separator = last_part.find_last_of("\\");  // Check for Windows

		if (last_dir_separator == std::string::npos) {
			last_dir_separator = last_part.find_last_of("/"); // Unix like

			if (last_dir_separator == std::string::npos) {
				return last_part;
			}
		}

		last_part = last_part.substr(last_dir_separator + 1);
		return last_part;
	}


	#ifdef _MSC_VER
	#include <intrin.h>
	#define sm_assert(x) do {if (!(x)) { __debugbreak();}} while(false)
	#else
	#define sm_assert(x) assert(x)
	#endif


	#define ProfileBlockStrS(x) Profiler _csgo_sdk_profiler_##__COUNTER__ (__FUNCTION__, x)
	#define ProfileBlockStrMS(x) Profiler _csgo_sdk_profiler_##__COUNTER__ (__FUNCTION__, x, Profiler::Scale::MILLISECONDS)
	#define ProfileBlockStrUS(x) Profiler _csgo_sdk_profiler_##__COUNTER__(__FUNCTION__, x, Profiler::Scale::MICROSECONDS)
	#define ProfileBlockS() Profiler _csgo_sdk_profiler_##__COUNTER__(__FUNCTION__, "")
	#define ProfileBlockMS() Profiler _csgo_sdk_profiler_##__COUNTER__ (__FUNCTION__, "", Profiler::Scale::MILLISECONDS)
	#define ProfileBlockUS() Profiler _csgo_sdk_profiler_##__COUNTER__(__FUNCTION__, "", Profiler::Scale::MICROSECONDS)

	class Profiler {
	public:
		enum class Scale {
			SECONDS,
			MILLISECONDS,
			MICROSECONDS
		};

		Profiler(const char* function_name, const char* text_arg = "", Scale scale = Scale::MILLISECONDS) {
			std::string text = std::string(text_arg);
			scale_ = scale;

			this->function_name_ = std::string(function_name);

			if (text != "") {
				this->function_name_ = this->function_name_ + " " + text;
			}


			time1_ = std::chrono::high_resolution_clock::now();
		}

		~Profiler() {
			std::chrono::high_resolution_clock::time_point time2 = std::chrono::high_resolution_clock::now();
			if (scale_ == Scale::MICROSECONDS) {
				auto duration = std::chrono::duration_cast<std::chrono::microseconds>(time2 - time1_).count();
				std::cout << function_name_ << ": " << duration << "us" << std::endl;
			} else if (scale_ == Scale::MILLISECONDS) {
				auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(time2 - time1_).count();
				std::cout << function_name_ << ": " << duration << "ms" << std::endl;
			} else {
				auto duration = std::chrono::duration_cast<std::chrono::seconds>(time2 - time1_).count();
				std::cout << function_name_ << ": " << duration << "s" << std::endl;
			}
		}

	private:
		Profiler();
		Scale scale_;
		std::chrono::high_resolution_clock::time_point time1_;
		std::string function_name_;
	};


}

#endif