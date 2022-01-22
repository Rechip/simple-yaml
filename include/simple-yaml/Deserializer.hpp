
#ifndef __SIMPLE_YAML_DESERIALIZER_HPP__
#define __SIMPLE_YAML_DESERIALIZER_HPP__
#pragma once

#include <charconv>
#include <chrono>
#include <filesystem>
#include <string>
#include <type_traits>
#include <vector>

#if defined(__has_include) && __has_include(<magic_enum.hpp>)
#	include <magic_enum.hpp>
#endif

#include "Exception.hpp"
#include "Parser.hpp"

namespace simple_yaml {

template<typename T>
struct Deserializer {
	//static_assert(false, "Unsopported type, you need to implement Deserializer.");
};

template<typename T>
requires std::is_integral_v<T> || std::is_floating_point_v<T>
struct Deserializer<T> {
	static T deserialize(const YAML::Node& n, const std::string& path) {
		if (!n.IsDefined()) {
			throw MissingNode("Missing basic type node " + path, n.Mark());
		}
		if (!n.IsScalar()) {
			throw InvalidNodeType("Invalid node type " + path, n.Mark());
		}
		return n.as<T>();
	}
};

#if defined(__has_include) && __has_include(<magic_enum.hpp>)

template<typename T>
requires std::is_enum_v<T>
struct Deserializer<T> {
	static T deserialize(const YAML::Node& n, const std::string& path) {
		if (!n.IsDefined()) {
			throw MissingNode("Missing basic type node " + path, n.Mark());
		}
		if (!n.IsScalar()) {
			throw InvalidNodeType("Invalid node type " + path, n.Mark());
		}
		auto en = magic_enum::enum_cast<T>(n.as<std::string>());
		if (!en.has_value()) {
			std::string values;
			for (auto a : magic_enum::enum_names<T>()) {
				values += " " + std::string{a};
			}
			throw InvalidNodeType("Invalid enum value \"" + n.as<std::string>() + "\" (possible:" + values + ") at " + path, n.Mark());
		}
		return en.value();
	}
};

#endif

// chrono durations as int64_t
template<typename Rep, typename Period>
struct Deserializer<std::chrono::duration<Rep, Period>> {
	static std::chrono::duration<Rep, Period> deserialize(const YAML::Node& n, const std::string& path) {
		if (!n.IsDefined()) {
			throw MissingNode("Missing basic type node " + path, n.Mark());
		}
		if (!n.IsScalar()) {
			throw InvalidNodeType("Invalid node type " + path, n.Mark());
		}
		const auto string = n.as<std::string>();
		Rep        result;
		if (auto [ptr, ec] = std::from_chars(string.data(), string.data() + string.size(), result); ec == std::errc() && ptr == string.data() + string.size()) {
			return std::chrono::duration<Rep, Period>{result};
		}
		return DurationParser<std::chrono::duration<Rep, Period>>::parse(string);
	}
};

// Representable as a string
template<typename T>
requires std::is_same_v<T, std::string> || std::is_same_v<T, std::filesystem::path>
struct Deserializer<T> {
	static T deserialize(const YAML::Node& n, const std::string& path) {
		if (!n.IsDefined()) {
			throw MissingNode("Missing basic type node " + path, n.Mark());
		}
		if (!n.IsScalar()) {
			throw InvalidNodeType("Invalid node type " + path, n.Mark());
		}
		return n.as<std::string>();
	}
};

template<typename T, size_t N>
struct Deserializer<std::array<T, N>> {
	static std::array<T, N> deserialize(const YAML::Node& n, const std::string& path) {
		if (!n.IsDefined()) {
			throw MissingNode("Missing Array[" + std::to_string(N) + "] node " + path, n.Mark());
		}

		if (!n.IsSequence() || n.size() != N) {
			throw InvalidNode("Expected sequence of length " + std::to_string(N) + " at " + path, n.Mark());
		}

		std::array<T, N> ret;
		for (size_t i{0}; i < N; ++i) {
			ret[i] = Deserializer<T>::deserialize(n[i], path + "[" + std::to_string(i) + "]");
		}
		return ret;
	}
};

template<typename T>
struct Deserializer<std::vector<T>> {
	static std::vector<T> deserialize(const YAML::Node& n, const std::string& path) {
		if (!n.IsDefined()) {
			throw MissingNode("Missing sequence node " + path, n.Mark());
		}
		std::vector<T> ret;
		size_t         i{0};
		for (const auto& in : n) {
			ret.push_back(Deserializer<T>::deserialize(in, path + "[" + std::to_string(i++) + "]"));
		}
		return ret;
	}
};

// Associative containers
template<typename T>
requires std::is_destructible_v<typename T::key_type> || std::is_destructible_v<typename T::mapped_type>
struct Deserializer<T> {
	static T deserialize(const YAML::Node& n, const std::string& path) {
		if (!n.IsDefined()) {
			throw MissingNode("Missing basic type node " + path, n.Mark());
		}
		if (!n.IsMap()) {
			throw InvalidNodeType("Invalid node type " + path, n.Mark());
		}

		T result;
		for (YAML::const_iterator it = n.begin(); it != n.end(); ++it) {
			auto fullpath = path + "/" + it->first.as<std::string>();
			auto key      = simple_yaml::Deserializer<std::decay_t<typename T::key_type>>::deserialize(it->first, path);
			auto value    = simple_yaml::Deserializer<std::decay_t<typename T::mapped_type>>::deserialize(it->second, fullpath);
			result.emplace(std::move(key), std::move(value));
		}
		return result;
	}
};

} // namespace simple_yaml

#endif
