#pragma once
#ifndef __RECHIP_YAML_SIMPLE_HPP__
#	define __RECHIP_YAML_SIMPLE_HPP__

#	include <any>
#	include <filesystem>
#	include <optional>
#	include <string>
#	include <type_traits>
#	include <yaml-cpp/yaml.h>

#	include "../pretty_name.hpp"
#	include "Exception.hpp"

namespace rechip::yaml {

constexpr auto fromFile   = YAML::LoadFile;
constexpr auto fromString = static_cast<YAML::Node (*)(const std::string&)>(YAML::Load);
constexpr auto fromStream = static_cast<YAML::Node (*)(std::istream&)>(YAML::Load);

struct MissingNode : RuntimeError {
	using RuntimeError::RuntimeError;
};

struct InvalidNodeType : RuntimeError {
	using RuntimeError::RuntimeError;
};

struct InvalidNode : RuntimeError {
	using RuntimeError::RuntimeError;
};

struct InvalidDefaultValue : RuntimeError {
	using RuntimeError::RuntimeError;
};

struct Simple;
struct Field;

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

#	if defined(__has_include) && __has_include(<magic_enum.hpp>)
#		include <magic_enum.hpp>

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
			throw InvalidNode("Invalid enum value (possible:" + values + ") at " + path, n.Mark());
		}
		return en.value();
	}
};

#	endif

template<typename T>
requires std::is_base_of_v<Simple, T>
struct Deserializer<T> {
	static T deserialize(const YAML::Node& n, const std::string& path) {
		if (!n.IsDefined()) {
			throw MissingNode("Missing structured type node " + path, n.Mark());
		}
		if (!n.IsMap()) {
			throw InvalidNodeType("Invalid node type " + path, n.Mark());
		}
		return T{n, path};
	}
};

// chrono durations as int64_t
template<typename T>
requires std::is_same_v<T, std::chrono::seconds> || std::is_same_v<T, std::chrono::milliseconds>
struct Deserializer<T> {
	static T deserialize(const YAML::Node& n, const std::string& path) {
		if (!n.IsDefined()) {
			throw MissingNode("Missing basic type node " + path, n.Mark());
		}
		if (!n.IsScalar()) {
			throw InvalidNodeType("Invalid node type " + path, n.Mark());
		}
		return T(n.as<std::int64_t>());
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

struct Field {
	Field(const ::YAML::Node& n, const std::string& path) : _data(n), _path(path) {
	}

	template<typename T>
	operator T() {
		if (!_data.IsDefined()) {
			if (_defaultValue.has_value()) {
				try {
					if constexpr (std::is_same_v<T, std::string>) {
						if (_defaultValue.type() == typeid(const char*)) {
							return T{std::any_cast<const char*>(_defaultValue)};
						}
					}

					return std::any_cast<T>(_defaultValue);
				} catch (const std::bad_any_cast& e) {
					throw InvalidDefaultValue("Invalid default value type for " + _path + ". It has to be '" + pretty_name<T>() + "'", _data.Mark());
				}
			}
			throw MissingNode("Missing node " + _path, _data.Mark());
		}
		return Deserializer<T>::deserialize(_data, _path);
	}

	Field& init(std::any defVal) {
		_defaultValue = defVal;
		return *this;
	}

private:
	YAML::Node  _data;
	std::any    _defaultValue;
	std::string _path;
};

struct Simple {
	Simple(const YAML::Node& n, const std::string& path = "") : _data(n), _path(path) {
	}

	Field bound(const std::string& key) {
		return Field{_data[key], _path + "/" + key};
	}

private:
	YAML::Node  _data;
	std::string _path;
};
} // namespace rechip::yaml

#endif // __RECHIP_YAML_SIMPLE_HPP__
