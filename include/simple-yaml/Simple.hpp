#pragma once
#ifndef __SIMPLE_YAML_SIMPLE_HPP__
#	define __SIMPLE_YAML_SIMPLE_HPP__

#	include <any>
#	include <filesystem>
#	include <optional>
#	include <string>
#	include <type_traits>
#	include <yaml-cpp/yaml.h>
#	include <pretty-name/pretty_name.hpp>

#	include "Exception.hpp"
#	include "Deserializer.hpp"
#	include "Field.hpp"

namespace simple_yaml {

constexpr auto fromFile   = YAML::LoadFile;
constexpr auto fromString = static_cast<YAML::Node (*)(const std::string&)>(YAML::Load);
constexpr auto fromStream = static_cast<YAML::Node (*)(std::istream&)>(YAML::Load);

struct Simple {
	Simple(const YAML::Node& n, const std::string& path = "") : _data(n), _path(path) {
	}
	Simple(const Simple& other) = default;
	Simple(Simple&& other)      = default;
	Simple& operator=(const Simple& other) = default;
	Simple& operator=(Simple&& other) = default;

	inline Field<void*> bound(const std::string& key) {
		return Field<void*>{_data[key], _path + "/" + key};
	}

	template<typename T>
	inline Field<T> bound(const std::string& key, const T& defVal) {
		return Field<T>{_data[key], _path + "/" + key}.init(defVal);
	}

	inline Field<std::string> bound(const std::string& key, const char* defVal) {
		return Field<std::string>{_data[key], _path + "/" + key}.init(std::string{defVal});
	}

private:
	YAML::Node  _data;
	std::string _path;
};

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

} // namespace simple_yaml

#endif // __SIMPLE_YAML_SIMPLE_HPP__
