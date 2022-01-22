# simple-yaml

Parsing configuration files is not fun. You usually want the config file to represent your inner structure. Thats where `simple-yaml` header-only library comes in.

## Features

- Parsing from string or a file.
- Many types of data are supported, see [data types](#data-types).
- Custom data types
- Embedded duration parser (eg. `1d 2h 3m 4s` to std::chrono::duration<>)
- Validation rules (minimum and maximum values, regex, length, ...)

## Requirements
- [yaml-cpp](https://github.com/jbeder/yaml-cpp)
- [source_location](https://github.com/Rechip/source_location) - you can use the standard implementation, this projects just extends compiler support
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
Create a configuration structure, inherit from `simple_yaml::Simple` and expose original constructor. Then you can `bound` any parameters. If your YAML configuration file looks like 
```yaml
username: Foo
password: Bar
```
then your C++ structure would look like
```cpp
struct Configuration : simple_yaml::Simple {
    using Simple::Simple;

    std::string user = bound("username");
    std::string pass = bound("password");
};
```
and parsing looks like
```cpp
Configuration config{ simple_yaml::fromFile("config.yaml") };
```

## Data types

Many types can be parsed from the configuration file.

- boolean
- enum (when conan config `enable_enum` is set or when `magic_enum.hpp` header is available)
- numeric types
- std::string
- std::filesystem::path
- std::chrono::duration
- std::array&lt;T&gt;
- std::vector&lt;T&gt;
- any type inheriting from `simple_yaml::simple`
- associative containers (std::map, std::unordered_map, std::set, std::unordered_set, ...)

## Default values

It is oftenusefull to have some predefined values. Just `init` them.
```cpp
int age = bound("age", 0);
```
I have used `std::any` to store the default value until you make conversion to the desired data type. This has one drawback and it is that your default value will have to exactly match the type you will cast to. This is not allowed:
```cpp
int age = bound("age", 0.0);  // ERROR you init `double`, but cast to `int`
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
#include <simple-yaml/simple_yaml.hpp>
using namespace simple_yaml;

struct Output : Simple {
    using Simple::Simple;

    std::filesystem::path file   = bound("file");
    bool                  pretty = bound("pretty", false);
};

struct Server : Simple {
    using Simple::Simple;

    std::string           url   = bound("url");
    std::vector<uint16_t> ports = bound("ports");
    bool                  https = bound("isSecure", true);
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

## Duration - std::chrono::duration<>
Since I often write network applications, I often want to parse timeouts.

```yaml
timeout: 1d 2h 30m 10s
```

```cpp
struct Configuration : Simple {
    using Simple::Simple;

    std::chrono::seconds timeout = bound("timeout");
};
```

Supported units are:
- `ns` - nanoseconds
- `us` - microseconds
- `ms` - miliseconds
- `s` - seconds
- `m` - minutes
- `h` - hours
- `d` - days
- `w` - weeks
- `mo` or `M` - months
- `y` - years

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

# Rules

On many ocasions you want to parse and validate your configuration before you start using it. Eg. in a network application the port number must be of maximum value 65535.

```cpp
struct Configuration : Simple {
    using Simple::Simple;

    uint16_t port           = bound("port").addRuleMaximum(65535);
    uint16_t age            = bound("age").addRuleMinimum(18, "You must be at least 18 years old to drink a beer in Europe");
    double angle            = bound("angle").addRuleRange(0.0, 360.0);
    std::string phoneNumber = bound("phoneNumber").addRuleLength(5, 30).addRuleRegex("^\\d+$");

    std::string fruit 		= bound("fruit").addRule([](std::string s){ return s == "apple" || s == "orange"; });
};
```

# Associative containers
If you want to for example parse a conandata.yml file, you can use the following code:

```yaml
sources:
  "0.2.0":
    url: "https://github.com/Rechip/source_location/archive/refs/tags/v0.2.0.zip"
    sha256: "60ea8261389e4d835eabb1d247a999a59a47733657d20654bac560cef8a63b9c"
  "0.1.1":
    url: "https://github.com/Rechip/source_location/archive/refs/tags/v0.1.1.zip"
    sha256: "2e177025c77d96cf9ccd994008dc5281e2385e1ee1bc89ca261807ce3f32c692"
```

```cpp
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

```

| :warning: LIMITATION          |
|:------------------------------|
| Class Simple is not default-constructible and any usage of `operator[]` on associative containers will produce code requiring a default-constructible value type. Use only `.at` function to access values.

# Kudos

This is a mere wrapper around [yaml-cpp](https://github.com/jbeder/yaml-cpp) library. All the heavy lifting is done there. Real kudos.

# TODO
- [ ] Optimize (storing copies of the YAML::Node)
- [x] Add more std:: types
- [x] Add validation rules
- [ ] Add more tests
