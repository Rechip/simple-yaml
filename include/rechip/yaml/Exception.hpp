#pragma once
#ifndef __RECHIP_YAML_EXCEPTION_HPP__
#	define __RECHIP_YAML_EXCEPTION_HPP__

#	include <source_location>
#	include <stdexcept>
#	include <yaml-cpp/yaml.h>

namespace rechip::yaml {

class Exception : public std::exception {
public:
	virtual std::string location() const     = 0;
	virtual std::string yamlLocation() const = 0;
};

template<typename Parent>
class BaseException : public Exception, public Parent {
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

	virtual ~BaseException() = default;

private:
	std::string          _what;
	::YAML::Mark         _mark;
	std::source_location _loc;
};

inline std::ostream& operator<<(std::ostream& os, const Exception& e) {
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

struct InvalidDefaultValue : RuntimeError {
	using RuntimeError::RuntimeError;
};

} // namespace rechip::yaml

#endif // __RECHIP_YAML_EXCEPTION_HPP__
