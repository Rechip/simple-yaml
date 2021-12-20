#pragma once
#ifndef __RECHIP_YAML_SIMPLE_HPP__
#	define __RECHIP_YAML_SIMPLE_HPP__

#	include <any>
#	include <filesystem>
#	include <optional>
#	include <string>
#	include <type_traits>
#	include <yaml-cpp/yaml.h>

#	include <rechip/pretty_name.hpp>
#	include "Exception.hpp"
#	include "Deserializer.hpp"
#	include "Field.hpp"

namespace rechip::yaml {

constexpr auto fromFile   = YAML::LoadFile;
constexpr auto fromString = static_cast<YAML::Node (*)(const std::string&)>(YAML::Load);
constexpr auto fromStream = static_cast<YAML::Node (*)(std::istream&)>(YAML::Load);

struct Simple {
	Simple(const YAML::Node& n, const std::string& path = "") : _data(n), _path(path) {
	}
	Simple(const Simple& other) = default;
	Simple(Simple&& other)      = default;

	Field bound(const std::string& key) {
		return Field{_data[key], _path + "/" + key};
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

} // namespace rechip::yaml

#endif // __RECHIP_YAML_SIMPLE_HPP__
