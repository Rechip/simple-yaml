#include "simple-yaml/simple_yaml.hpp"
#include <gtest/gtest.h>
#include <string>

using namespace simple_yaml;

static const std::string source{R"(
empty: ""
oneWord: one
multipleWords: one two three
specialChars: "!@#$%^&*()_+-=[]{}|:;',./<>?"
upperCase: ONE
)"};

TEST(String, Equality) {
	const struct : Simple {
		using Simple::Simple;

		std::string empty         = bound("empty");
		std::string oneWord       = bound("oneWord");
		std::string multipleWords = bound("multipleWords");
		std::string specialChars  = bound("specialChars");
		std::string upperCase     = bound("upperCase");
	} config{fromString(source)};

	EXPECT_EQ(config.empty, "");
	EXPECT_EQ(config.oneWord, "one");
	EXPECT_EQ(config.multipleWords, "one two three");
	EXPECT_EQ(config.specialChars, "!@#$%^&*()_+-=[]{}|:;',./<>?");
	EXPECT_EQ(config.upperCase, "ONE");
}

TEST(String, Missing) {
	auto missing = [] {
		struct : Simple {
			std::string missing = bound("missing");
		} config{fromString(source)};
	};

	EXPECT_THROW(missing(), simple_yaml::MissingNode);
}

TEST(String, DefaultValue) {
	const struct : Simple {
		using Simple::Simple;

		std::string missingString = bound("missingString", std::string{"defaultString"});
		std::string missingChars  = bound("missingChars", "defaultChars");
	} config{fromString(source)};

	EXPECT_EQ(config.missingString, "defaultString");
	EXPECT_EQ(config.missingChars, "defaultChars");
}

TEST(String, InvalidType) {
	// Almost anything can be cast to string
	const std::string invalidSource{R"(
int: 1
float: 3.14
double: 1.618
bool: false
char: c
long: 1234567890123456789
)"};

	auto invalidFromInt = [&] {
		struct : Simple {
			std::string invalid = bound("int");
		} config{fromString(invalidSource)};
	};
	EXPECT_NO_THROW(invalidFromInt());

	auto invalidFromFloat = [&] {
		struct : Simple {
			std::string invalid = bound("float");
		} config{fromString(invalidSource)};
	};
	EXPECT_NO_THROW(invalidFromFloat());

	auto invalidFromDouble = [&] {
		struct : Simple {
			std::string invalid = bound("double");
		} config{fromString(invalidSource)};
	};
	EXPECT_NO_THROW(invalidFromDouble());

	auto invalidFromBool = [&] {
		struct : Simple {
			std::string invalid = bound("bool");
		} config{fromString(invalidSource)};
	};
	EXPECT_NO_THROW(invalidFromBool());

	auto invalidFromChar = [&] {
		struct : Simple {
			std::string invalid = bound("char");
		} config{fromString(invalidSource)};
	};
	EXPECT_NO_THROW(invalidFromChar());

	auto invalidFromLong = [&] {
		struct : Simple {
			std::string invalid = bound("long");
		} config{fromString(invalidSource)};
	};
	EXPECT_NO_THROW(invalidFromLong());
}
