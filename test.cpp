#include "rechip/yaml.hpp"

using namespace rechip::yaml;

const std::string simpleYAML{R"(
string: foo bar
int: 15
bool: true
)"};

struct SimpleConfig : Simple {
	using Simple::Simple;

	std::string str     = bound("string");
	int         integer = bound("int");
	bool        boolean = bound("bool");
};

int main() {
	SimpleConfig config{fromString(simpleYAML)};

	assert(config.str == "foo bar");
	assert(config.integer == 15);
	assert(config.boolean);

	return 0;
}
