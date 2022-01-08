#include "simple-yaml/simple_yaml.hpp"
#include <gtest/gtest.h>
#include <string>

using namespace simple_yaml;

static const std::string source{R"(
int: 10
float: 3.14
double: 1.618
bool: false
char: c
long: 1234567890123456789
)"};

TEST(Primitives, Equality) {
	const struct : Simple {
		using Simple::Simple;

		int       integer        = bound("int");
		float     floating       = bound("float");
		double    doublefloating = bound("double");
		bool      boolean        = bound("bool");
		char      character      = bound("char");
		long long longLong       = bound("long");

	} config{fromString(source)};

	EXPECT_EQ(config.integer, 10);
	EXPECT_FLOAT_EQ(config.floating, 3.14f);
	EXPECT_DOUBLE_EQ(config.doublefloating, 1.618);
	EXPECT_EQ(config.boolean, false);
	EXPECT_EQ(config.character, 'c');
	EXPECT_EQ(config.longLong, static_cast<long long>(1234567890123456789));
}

TEST(Primitives, Missing) {
	auto missingInt = [] {
		struct : Simple {
			int missing = bound("missing");
		} config{fromString(source)};
	};

	EXPECT_THROW(missingInt(), simple_yaml::MissingNode);

	auto missingFloat = [] {
		struct : Simple {
			float missing = bound("missing");
		} config{fromString(source)};
	};

	EXPECT_THROW(missingFloat(), simple_yaml::MissingNode);

	auto missingDouble = [] {
		struct : Simple {
			double missing = bound("missing");
		} config{fromString(source)};
	};

	EXPECT_THROW(missingDouble(), simple_yaml::MissingNode);

	auto missingBool = [] {
		struct : Simple {
			bool missing = bound("missing");
		} config{fromString(source)};
	};

	EXPECT_THROW(missingBool(), simple_yaml::MissingNode);

	auto missingChar = [] {
		struct : Simple {
			char missing = bound("missing");
		} config{fromString(source)};
	};

	EXPECT_THROW(missingChar(), simple_yaml::MissingNode);

	auto missingLong = [] {
		struct : Simple {
			long long missing = bound("missing");
		} config{fromString(source)};
	};

	EXPECT_THROW(missingLong(), simple_yaml::MissingNode);
}

TEST(Primitives, DefaultValue) {
	const struct : Simple {
		using Simple::Simple;

		int       missingInt    = bound("missingInt", 15);
		float     missingFloat  = bound("missingFloat", -3.14);
		double    missingDouble = bound("missingDouble", -1.618);
		bool      missingBool   = bound("missingBool", true);
		char      missingChar   = bound("missingChar", 'z');
		long long missingLong   = bound("missingLong", static_cast<long long>(-1234567890123456789));
	} config{fromString(source)};

	EXPECT_EQ(config.missingInt, 15);
	EXPECT_FLOAT_EQ(config.missingFloat, -3.14f);
	EXPECT_DOUBLE_EQ(config.missingDouble, -1.618);
	EXPECT_EQ(config.missingBool, true);
	EXPECT_EQ(config.missingChar, 'z');
	EXPECT_EQ(config.missingLong, static_cast<long long>(-1234567890123456789));
}

TEST(Primitives, InvalidToInt) {
	auto invalidFromFloat = [] {
		struct : Simple {
			int invalid = bound("float");
		} config{fromString(source)};
	};

	auto invalidFromDouble = [] {
		struct : Simple {
			int invalid = bound("double");
		} config{fromString(source)};
	};

	auto invalidFromBool = [] {
		struct : Simple {
			int invalid = bound("bool");
		} config{fromString(source)};
	};

	auto invalidFromChar = [] {
		struct : Simple {
			int invalid = bound("char");
		} config{fromString(source)};
	};

	auto invalidFromLong = [] {
		struct : Simple {
			int invalid = bound("long");
		} config{fromString(source)};
	};

	EXPECT_THROW(invalidFromFloat(), YAML::TypedBadConversion<int>);
	EXPECT_THROW(invalidFromDouble(), YAML::TypedBadConversion<int>);
	EXPECT_THROW(invalidFromBool(), YAML::TypedBadConversion<int>);
	EXPECT_THROW(invalidFromChar(), YAML::TypedBadConversion<int>);
	EXPECT_THROW(invalidFromLong(), YAML::TypedBadConversion<int>);
}

TEST(Primitives, InvalidToBool) {
	auto invalidFromInt = [] {
		struct : Simple {
			bool invalid = bound("int");
		} config{fromString(source)};
	};

	auto invalidFromFloat = [] {
		struct : Simple {
			bool invalid = bound("float");
		} config{fromString(source)};
	};

	auto invalidFromDouble = [] {
		struct : Simple {
			bool invalid = bound("double");
		} config{fromString(source)};
	};

	auto invalidFromChar = [] {
		struct : Simple {
			bool invalid = bound("char");
		} config{fromString(source)};
	};

	auto invalidFromLong = [] {
		struct : Simple {
			bool invalid = bound("long");
		} config{fromString(source)};
	};

	EXPECT_THROW(invalidFromInt(), YAML::TypedBadConversion<bool>);
	EXPECT_THROW(invalidFromFloat(), YAML::TypedBadConversion<bool>);
	EXPECT_THROW(invalidFromDouble(), YAML::TypedBadConversion<bool>);
	EXPECT_THROW(invalidFromChar(), YAML::TypedBadConversion<bool>);
	EXPECT_THROW(invalidFromLong(), YAML::TypedBadConversion<bool>);
}

TEST(Primitives, InvalidToFloat) {
	auto invalidFromInt = [] {
		struct : Simple {
			float invalid = bound("int");
		} config{fromString(source)};
	};

	auto invalidFromBool = [] {
		struct : Simple {
			float invalid = bound("bool");
		} config{fromString(source)};
	};

	auto invalidFromDouble = [] {
		struct : Simple {
			float invalid = bound("double");
		} config{fromString(source)};
	};

	auto invalidFromChar = [] {
		struct : Simple {
			float invalid = bound("char");
		} config{fromString(source)};
	};

	auto invalidFromLong = [] {
		struct : Simple {
			float invalid = bound("long");
		} config{fromString(source)};
	};

	EXPECT_NO_THROW(invalidFromInt());
	EXPECT_THROW(invalidFromBool(), YAML::TypedBadConversion<float>);
	EXPECT_NO_THROW(invalidFromDouble());
	EXPECT_THROW(invalidFromChar(), YAML::TypedBadConversion<float>);
	EXPECT_NO_THROW(invalidFromLong());
}

TEST(Primitives, InvalidToDouble) {
	auto invalidFromInt = [] {
		struct : Simple {
			double invalid = bound("int");
		} config{fromString(source)};
	};

	auto invalidFromBool = [] {
		struct : Simple {
			double invalid = bound("bool");
		} config{fromString(source)};
	};

	auto invalidFromFloat = [] {
		struct : Simple {
			double invalid = bound("float");
		} config{fromString(source)};
	};

	auto invalidFromChar = [] {
		struct : Simple {
			double invalid = bound("char");
		} config{fromString(source)};
	};

	auto invalidFromLong = [] {
		struct : Simple {
			double invalid = bound("long");
		} config{fromString(source)};
	};

	EXPECT_NO_THROW(invalidFromInt());
	EXPECT_THROW(invalidFromBool(), YAML::TypedBadConversion<double>);
	EXPECT_NO_THROW(invalidFromFloat());
	EXPECT_THROW(invalidFromChar(), YAML::TypedBadConversion<double>);
	EXPECT_NO_THROW(invalidFromLong());
}

TEST(Primitives, InvalidToChar) {
	auto invalidFromInt = [] {
		struct : Simple {
			char invalid = bound("int");
		} config{fromString(source)};
	};

	auto invalidFromBool = [] {
		struct : Simple {
			char invalid = bound("bool");
		} config{fromString(source)};
	};

	auto invalidFromFloat = [] {
		struct : Simple {
			char invalid = bound("float");
		} config{fromString(source)};
	};

	auto invalidFromDouble = [] {
		struct : Simple {
			char invalid = bound("double");
		} config{fromString(source)};
	};

	auto invalidFromLong = [] {
		struct : Simple {
			char invalid = bound("long");
		} config{fromString(source)};
	};

	EXPECT_THROW(invalidFromInt(), YAML::TypedBadConversion<char>);
	EXPECT_THROW(invalidFromBool(), YAML::TypedBadConversion<char>);
	EXPECT_THROW(invalidFromFloat(), YAML::TypedBadConversion<char>);
	EXPECT_THROW(invalidFromDouble(), YAML::TypedBadConversion<char>);
	EXPECT_THROW(invalidFromLong(), YAML::TypedBadConversion<char>);
}

TEST(Primitives, InvalidToLong) {
	auto invalidFromInt = [] {
		struct : Simple {
			long invalid = bound("int");
		} config{fromString(source)};
	};

	auto invalidFromBool = [] {
		struct : Simple {
			long invalid = bound("bool");
		} config{fromString(source)};
	};

	auto invalidFromFloat = [] {
		struct : Simple {
			long invalid = bound("float");
		} config{fromString(source)};
	};

	auto invalidFromDouble = [] {
		struct : Simple {
			long invalid = bound("double");
		} config{fromString(source)};
	};

	auto invalidFromChar = [] {
		struct : Simple {
			long invalid = bound("char");
		} config{fromString(source)};
	};

	EXPECT_NO_THROW(invalidFromInt());
	EXPECT_THROW(invalidFromBool(), YAML::TypedBadConversion<long>);
	EXPECT_THROW(invalidFromFloat(), YAML::TypedBadConversion<long>);
	EXPECT_THROW(invalidFromDouble(), YAML::TypedBadConversion<long>);
	EXPECT_THROW(invalidFromChar(), YAML::TypedBadConversion<long>);
}
