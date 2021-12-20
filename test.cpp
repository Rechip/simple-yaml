#include "rechip/yaml.hpp"

using namespace rechip::yaml;

const std::string simpleYAML{R"(
string: foo bar
int: 15
bool: true
)"};

const std::string simpleYAMLUppercase{R"(
string: FOO BAR
int: 15
bool: true
)"};

struct SimpleConfig : Simple {
	using Simple::Simple;

	std::string str     = bound("string");
	int         integer = bound("int");
	bool        boolean = bound("bool");
};

struct SimpleConfigUppercase : Simple {
	using Simple::Simple;

	std::string str     = bound("string").addRuleRegex("[A-Z\\s]+", "All characters must be uppercase");
	int         integer = bound("int");
	bool        boolean = bound("bool");
};

int main() {
	SimpleConfig config{fromString(simpleYAML)};

	assert(config.str == "foo bar");
	assert(config.integer == 15);
	assert(config.boolean);

	bool caught = false;
	try {
		SimpleConfigUppercase cfg{fromString(simpleYAML)};
	} catch (const ValidatorFailed& e) {
		caught = true;
	}
	assert(caught);

	SimpleConfigUppercase cfg{fromString(simpleYAMLUppercase)};
	assert(cfg.str == "FOO BAR");
	assert(cfg.integer == 15);
	assert(cfg.boolean);

	return 0;
}
