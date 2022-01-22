#include "simple-yaml/simple_yaml.hpp"
#include <gtest/gtest.h>
#include <string>

#include <map>
#include <unordered_map>

using namespace simple_yaml;

static const std::string source{R"(
sources:
  "0.2.0":
    url: "https://github.com/Rechip/source_location/archive/refs/tags/v0.2.0.zip"
    sha256: "60ea8261389e4d835eabb1d247a999a59a47733657d20654bac560cef8a63b9c"
  "0.1.1":
    url: "https://github.com/Rechip/source_location/archive/refs/tags/v0.1.1.zip"
    sha256: "2e177025c77d96cf9ccd994008dc5281e2385e1ee1bc89ca261807ce3f32c692"
)"};

struct VersionSource : Simple {
	using Simple::Simple;

	std::string url    = bound("url");
	std::string sha256 = bound("sha256").addRuleLength(64, 64, "sha256sum muset be exactly 64 characters long");
};

struct Configuration : Simple {
	using Simple::Simple;

	std::map<std::string, VersionSource>                versions                    = bound("sources");
	std::unordered_map<std::string, VersionSource>      unordered_versions          = bound("sources");
	std::multimap<std::string, VersionSource>           multimap_versions           = bound("sources");
	std::unordered_multimap<std::string, VersionSource> unordered_multimap_versions = bound("sources");
};

TEST(Map, Equality) {
	const Configuration config{fromString(source)};

	EXPECT_TRUE(config.versions.contains("0.2.0"));
	EXPECT_TRUE(config.versions.contains("0.1.1"));
	EXPECT_EQ(config.versions.at("0.2.0").url, "https://github.com/Rechip/source_location/archive/refs/tags/v0.2.0.zip");
	EXPECT_EQ(config.versions.at("0.1.1").sha256, "2e177025c77d96cf9ccd994008dc5281e2385e1ee1bc89ca261807ce3f32c692");

	EXPECT_TRUE(config.unordered_versions.contains("0.2.0"));
	EXPECT_TRUE(config.unordered_versions.contains("0.1.1"));
	EXPECT_EQ(config.unordered_versions.at("0.2.0").url, "https://github.com/Rechip/source_location/archive/refs/tags/v0.2.0.zip");
	EXPECT_EQ(config.unordered_versions.at("0.1.1").sha256, "2e177025c77d96cf9ccd994008dc5281e2385e1ee1bc89ca261807ce3f32c692");

	EXPECT_TRUE(config.multimap_versions.contains("0.2.0"));
	EXPECT_TRUE(config.multimap_versions.contains("0.1.1"));
	EXPECT_EQ(config.multimap_versions.find("0.2.0")->second.url, "https://github.com/Rechip/source_location/archive/refs/tags/v0.2.0.zip");
	EXPECT_EQ(config.multimap_versions.find("0.1.1")->second.sha256, "2e177025c77d96cf9ccd994008dc5281e2385e1ee1bc89ca261807ce3f32c692");

	EXPECT_TRUE(config.unordered_multimap_versions.contains("0.2.0"));
	EXPECT_TRUE(config.unordered_multimap_versions.contains("0.1.1"));
	EXPECT_EQ(config.unordered_multimap_versions.find("0.2.0")->second.url, "https://github.com/Rechip/source_location/archive/refs/tags/v0.2.0.zip");
	EXPECT_EQ(config.unordered_multimap_versions.find("0.1.1")->second.sha256, "2e177025c77d96cf9ccd994008dc5281e2385e1ee1bc89ca261807ce3f32c692");
}
