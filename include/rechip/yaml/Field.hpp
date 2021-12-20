#pragma once
#ifndef __RECHIP_YAML_FIELD_HPP__
#	define __RECHIP_YAML_FIELD_HPP__

#	include <type_traits>
#	include <yaml-cpp/yaml.h>
#	include <regex>
#	include "Deserializer.hpp"

namespace rechip::yaml {

struct ValidatorFailed : RuntimeError {
	using RuntimeError::RuntimeError;
};

template<typename T>
concept deserializable_v = !std::is_same_v<void, decltype(Deserializer<T>::deserialize(YAML::Node{}, std::string{}))>;

struct Field {
	Field(const ::YAML::Node& n, const std::string& path) : _data(n), _path(path) {
	}

	template<typename T>
	requires deserializable_v<T> T convertTo()
	const {
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

	template<typename T>
	requires deserializable_v<T>
	operator T() {
		validate();
		return convertTo<T>();
	}

	Field& init(std::any defVal) {
		_defaultValue = defVal;
		return *this;
	}

	template<typename T>
	Field& addRule(std::function<bool(T)> predicate, const std::string& errMsg = "") {
		_rules.emplace_back([predicate, errMsg, this] {
			T value = convertTo<T>();
			if (!predicate(value)) {
				throw ValidatorFailed(errMsg.empty() ? "Validation failed for " + _path : errMsg, _data.Mark());
			}
		});
		return *this;
	}

	Field& addRuleRegex(std::regex regex, const std::string& errMsg = "") {
		return addRule<std::string>([regex](const std::string& value) { return std::regex_match(value, regex); }, errMsg);
	}

	Field& addRuleRegex(std::string regex, const std::string& errMsg = "") {
		return addRuleRegex(std::regex{regex}, errMsg);
	}

	template<typename T>
	Field& addRuleRange(T min, T max, const std::string& errMsg = "") {
		return addRule<T>([min, max](const T& value) { return min <= value && value <= max; });
	}

	template<typename T>
	Field& addRuleMinimum(T min, const std::string& errMsg = "") {
		return addRuleRange<T>(min, std::numeric_limits<T>::max(), errMsg);
	}

	template<typename T>
	Field& addRuleMaximum(T max, const std::string& errMsg = "") {
		return addRuleRange<T>(std::numeric_limits<T>::min(), max, errMsg);
	}

	Field& addRuleLength(size_t min, size_t max, const std::string& errMsg = "") {
		return addRule<std::string>([min, max](const std::string& value) { return min <= value.size() && value.size() <= max; }, errMsg);
	}

	Field& addRuleLengthMinimum(size_t min, const std::string& errMsg = "") {
		return addRuleLength(min, std::numeric_limits<size_t>::max(), errMsg);
	}

	Field& addRuleLengthMaximum(size_t max, const std::string& errMsg = "") {
		return addRuleLength(std::numeric_limits<size_t>::min(), max, errMsg);
	}

	void validate() {
		for (const auto& rule : _rules) {
			rule();
		}
	}

private:
	YAML::Node                         _data;
	std::any                           _defaultValue;
	std::string                        _path;
	std::vector<std::function<void()>> _rules;
};
} // namespace rechip::yaml

#endif
