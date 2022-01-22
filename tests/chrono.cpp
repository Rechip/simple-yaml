#include "simple-yaml/simple_yaml.hpp"
#include <gtest/gtest.h>
#include <string>

using namespace simple_yaml;

static const std::string source{R"(
milli: 15
dayLong: 1d
dayShort: 24h
)"};

TEST(Chrono, Equality) {
	const struct : Simple {
		using Simple::Simple;

		std::chrono::milliseconds milli    = bound("milli");
		std::chrono::seconds      dayLong  = bound("dayLong");
		std::chrono::hours        dayShort = bound("dayShort");
	} config{fromString(source)};

	EXPECT_EQ(config.milli, std::chrono::milliseconds{15});
	EXPECT_EQ(config.dayLong, std::chrono::days{1});
	EXPECT_EQ(config.dayShort, std::chrono::minutes{24 * 60});
}
