#pragma once
#ifndef __SIMPLE_YAML_EXCEPTION_HPP__
#	define __SIMPLE_YAML_EXCEPTION_HPP__

#	include <source_location>
#	include <stdexcept>
#	include <yaml-cpp/yaml.h>

namespace simple_yaml {

class Exception {
public:
	virtual std::string location() const     = 0;
	virtual std::string yamlLocation() const = 0;

	virtual ~Exception() = default;
};

template<typename Parent>
class BaseException : public Parent, public Exception {
public:
	BaseException(const std::string& what, ::YAML::Mark mark, std::source_location loc = std::source_location::current())
	    : Parent(what), _what(what), _mark(mark), _loc(loc) {
	}

	const char* what() const noexcept override {
		return _what.c_str();
	}

	std::string location() const override {
		return std::string{_loc.file_name()} + ":" + _loc.function_name() + "@line " + std::to_string(_loc.line()) + ":" + std::to_string(_loc.column());
	}

	std::string yamlLocation() const override {
		return std::string{"@"} + std::to_string(_mark.line) + ":" + std::to_string(_mark.column);
	}

	~BaseException() override = default;

private:
	std::string          _what;
	::YAML::Mark         _mark;
	std::source_location _loc;
};

template<typename Parent>
inline std::ostream& operator<<(std::ostream& os, const BaseException<Parent>& e) {
	return os << e.what() << " " << e.yamlLocation();
}

using RuntimeError = BaseException<std::runtime_error>;

struct MissingNode : RuntimeError {
	using RuntimeError::RuntimeError;
};

struct InvalidNodeType : RuntimeError {
	using RuntimeError::RuntimeError;
};

struct InvalidNode : RuntimeError {
	using RuntimeError::RuntimeError;
};

} // namespace simple_yaml

#endif // __SIMPLE_YAML_EXCEPTION_HPP__
