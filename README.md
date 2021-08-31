# simple-yaml

Parsing configuration files is not fun. You usually want the config file to represent your inner structure. Thats where `simple-yaml` header-only library comes in. 

## Requirements
- [yaml-cpp](https://github.com/jbeder/yaml-cpp)
- [magic_enum](https://github.com/Neargye/magic_enum) [optional]
- C++20 - I am too lazy to deal with SFINAE, so I use concepts

If you use the conan package, you do not need to worry about dependencies, just c++ standard.

## Usage
It is a header-only library, so you can, you know, copy... the include directory. Or you can use conan to make a package:
```sh
conan create .
```
and then just specify dependencies in your another projects conanfile.txt
```txt
[requires]
simple-yaml/0.1
```

## Example simple
Create a configuration structure, inherit from `rechip::yaml::Simple` and expose original constructor. Then you can `bound` any parameters. If your YAML configuration file looks like 
```yaml
username: Foo
password: Bar
```
then your C++ structure would look like
```cpp
struct Configuration : rechip::yaml::Simple {
	using Simple::Simple;

	std::string user = bound("username");
	std::string pass = bounf("password");
};
```
and parsing looks like
```cpp
Configuration config{ rechip::yaml::fromFile("config.yaml") };
```

## Data types

Many types can be parsed from the configuration file.

- boolean
- enum (when conan config `enable_enum` is set or when `magic_enum.hpp` header is available)
- numeric types
- std::string
- std::filesystem::path
- std::chrono::seconds and std::chrono::milliseconds (as simple numeric types)
- std::array&lt;T&gt;
- std::vector&lt;T&gt;
- any type inheriting from `rechip::yaml::simple`

## Default values

It is oftenusefull to have some predefined values. Just `init` them.
```cpp
int age = bound("age").init(0);
```
I have used `std::any` to store the default value until you make conversion to the desired data type. This has one drawback and it is that your default value will have to exactly match the type you will cast to. This is not allowed:
```cpp
int age = bound("age").init(0.0);  // ERROR you init `double`, but cast to `int`
```

## More complex example
conf.yaml
```yaml
output:
  file: ./out
server:
  url: local.host
  ports: [1228, 1229, 1330]
  isSecure: false
```
main.cpp
```cpp
#include <iostream>
#include <rechip/yaml.hpp>
using namespace rechip::yaml;

struct Output : Simple {
	using Simple::Simple;

	std::filesystem::path file   = bound("file");
	bool                  pretty = bound("pretty").init(false);
};

struct Server : Simple {
	using Simple::Simple;

    std::string           url   = bound("url");
	std::vector<uint16_t> ports = bound("ports");
	bool                  https = bound("isSecure").init(true);
};

struct Configuration : Simple {
	using Simple::Simple;

    Output out    = bound("output");
	Server server = bound("server");
};

void main() {
	Configuration config {fromFile("conf.yaml")};
	std::cout << "Connecting to: " << config.server.url << std::endl;
};
```

## Custom deserializators
You can write your own parser for your custom types. Here is a example for the numeric types:
```cpp
template<typename T>
requires std::is_integral_v<T> || std::is_floating_point_v<T>
struct Deserializer<T> {
	static T deserialize(const YAML::Node& n, const std::string& path) {
		if (!n.IsDefined()) {
			throw MissingNode("Missing basic type node " + path, n.Mark());
		}
		if (!n.IsScalar()) {
			throw InvalidNodeType("Invalid node type " + path, n.Mark());
		}
		return n.as<T>();
	}
};
```
As simple as that.

# Kudos
This is a mere wrapper around [yaml-cpp](https://github.com/jbeder/yaml-cpp) library. All the heavy lifting is done there. Real kudos.

# TODO
- [ ] Optimize (storing copies of the YAML::Node)
- [ ] Add more std:: types
- [ ] Add more tests
