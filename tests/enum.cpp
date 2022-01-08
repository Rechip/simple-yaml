#include "simple-yaml/simple_yaml.hpp"
#include <gtest/gtest.h>
#include <string>

#if defined(__has_include) && __has_include(<magic_enum.hpp>)

using namespace simple_yaml;

enum class Enum1 { A, B, C };
enum class Enum2 { X, Y, Z };

static const std::string source{R"(
enum1_a: A
enum1_b: B
enum1_c: C
enum1_x: X 
enum1_y: Y
enum1_z: Z
enum2_x: X
enum2_y: Y
enum2_z: Z
)"};

TEST(Enum, Equality) {
	const struct : Simple {
		using Simple::Simple;

		Enum1 enum1_a = bound("enum1_a");
		Enum1 enum1_b = bound("enum1_b");
		Enum1 enum1_c = bound("enum1_c");
	} config{fromString(source)};

	EXPECT_EQ(config.enum1_a, Enum1::A);
	EXPECT_EQ(config.enum1_b, Enum1::B);
	EXPECT_EQ(config.enum1_c, Enum1::C);
}

TEST(Enum, Missing) {
	auto missing = [] {
		struct : Simple {
			Enum1 missing = bound("missing");
		} config{fromString(source)};
	};

	EXPECT_THROW(missing(), simple_yaml::MissingNode);
}

TEST(Enum, DefaultValue) {
	const struct : Simple {
		using Simple::Simple;

		Enum1 missingEnum1 = bound("missingEnum1", Enum1::C);
	} config{fromString(source)};

	EXPECT_EQ(config.missingEnum1, Enum1::C);
}

TEST(Enum, InvalidType) {
	auto invalidEnum = [] {
		struct : Simple {
			Enum1 invalid = bound("enum2_x");
		} config{fromString(source)};
	};

	EXPECT_THROW(invalidEnum(), simple_yaml::InvalidNodeType);
}

#endif
