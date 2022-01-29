#ifndef __SIMPLE_YAML_PARSER_HPP__
#define __SIMPLE_YAML_PARSER_HPP__
#pragma once

#include <chrono>
#include <regex>
#include <string_view>
#include <unordered_map>

namespace simple_yaml {

template<typename Duration>
class DurationParser {
	template<typename ParseDuration>
	static constexpr std::function<Duration(int64_t)> makeConverter() {
		return [](int64_t i) { return std::chrono::duration_cast<Duration>(ParseDuration{i}); };
	}

public:
	static Duration parse(std::string str) {
		static std::unordered_map<std::string_view, std::function<Duration(int64_t)>> converters{
		    {"ns", makeConverter<std::chrono::nanoseconds>()},
		    {"us", makeConverter<std::chrono::microseconds>()},
		    {"ms", makeConverter<std::chrono::milliseconds>()},
		    {"s", makeConverter<std::chrono::seconds>()},
		    {"m", makeConverter<std::chrono::minutes>()},
		    {"h", makeConverter<std::chrono::hours>()},
		    {"d", makeConverter<std::chrono::days>()},
		    {"w", makeConverter<std::chrono::weeks>()},
		    {"M", makeConverter<std::chrono::months>()},
		    {"mo", makeConverter<std::chrono::months>()},
		    {"y", makeConverter<std::chrono::years>()}};

		std::regex  regex("(-?[0-9]+)([a-zA-Z])+");
		std::smatch match;
		Duration    duration{Duration::zero()};

		while (std::regex_search(str, match, regex)) {
			if (!converters.contains(match[2].str())) {
				throw std::runtime_error("Unknown duration unit: " + match[2].str());
			}
			duration += converters[match[2].str()](std::stoll(match[1].str()));
			str = match.suffix().str();
		}

		return duration;
	}
};

} // namespace simple_yaml

#endif // __SIMPLE_YAML_PARSER_HPP__