#ifndef _CARBON_UTIL_UTIL_HPP_
#define _CARBON_UTIL_UTIL_HPP_

#include <codecvt>
#include <random>

#include "easing.hpp"
#include "input.hpp"
namespace carbon::util {
	inline std::string wide_to_multibyte(const std::wstring& str) {
		if (str.empty())
			return {};

		const size_t size = WideCharToMultiByte(CP_UTF8, 0, &str[0], str.size(), nullptr, 0, nullptr, nullptr);

		std::string ret;
		ret.resize(size);

		WideCharToMultiByte(CP_UTF8, 0, &str[0], str.size(), &ret[0], size, nullptr, nullptr);
		return ret;
	}
	
	inline std::wstring multibyte_to_wide(const std::string& str) {
		if (str.empty())
			return {};
		
		const size_t size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.size() + 1, nullptr, 0);

		std::wstring out;
		out.resize(size);
		
		MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.size() + 1, &out[0], size);
		return out;
	}

	// todo: implement easing into fade
	inline void fade(int& alpha, const bool in = true, const float time = 1.0f, const int min = 0, const int max = 255) {
		const auto step = static_cast<int>(255.0f / time * (ctx->io.frame_delta / 100.0f));
		alpha = std::clamp(in ? alpha + step : alpha - step, min, max);
	}

	template<typename ... Args>
	std::string format(const char* f, Args ... args) {
		char buffer[64];
		snprintf(buffer, sizeof buffer, f, args ...);
		return std::string(buffer);
	}
	
	template<typename T>
	T random(T min, T max) {
		static std::random_device rd;
		static std::mt19937 gen(rd());
		
		if constexpr (!std::is_integral_v<T>) {
			std::uniform_real_distribution<T> dist(min, max);
			return dist(gen);
		}
		else {
			std::uniform_int_distribution<T> dist(min, max);
			return dist(gen);
		}
	}

	/**
	 * \brief split thing for example abc###efg with the delimiter being ### would return { "abc", "efg" }
	 * \param phrase phrase to split
	 * \param delimiter what to split on
	 * \return split string
	 */
	inline std::vector<std::string> split_string(char* phrase, const std::string delimiter) {\
		std::vector<std::string> list;
		auto str = std::string(phrase);
		for (size_t i; (i = str.find(delimiter)) != std::string::npos;) {
			auto token = str.substr(0, i);
			list.push_back(token);
			str.erase(0, i + delimiter.length());
		}
		list.push_back(str);
		return list;
	}

	inline uint32_t hash(const std::string& str) {
		constexpr uint32_t b = 378551;
		
		uint32_t a = 63689;
		uint32_t hash = 0;

		for (auto i : str) {
			hash = hash * a + i;
			a = a * b;
		}

		return hash & 0x7FFFFFFF;
	}
}

#endif