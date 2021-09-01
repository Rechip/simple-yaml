import os
from conans import ConanFile, CMake, tools

class SimpleYamlConan(ConanFile):
    name = "simple-yaml"
    version = "0.1.1"
    license = "MIT"
    url = "https://github.com/Rechip/simple-yaml"
    description = "Read YAML configuration files by code structure"
    topics = ("configuration", "yaml", "cpp")
    settings = "os", "compiler", "arch", "build_type"
  
    generators = "cmake"
    exports_sources = "include/*", "CMakeLists.txt", "test.cpp"
    no_copy_source = True
    
    options = {
        "enable_enum": [True, False],
    }
    default_options = {
        "enable_enum": True
    }

    def build(self): # just tests
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        cmake.test()

    def validate(self):
        tools.check_min_cppstd(self, "20")

    def requirements(self):
        self.requires("yaml-cpp/0.7.0")
        if self.options.enable_enum:
            self.requires("magic_enum/0.7.3") 

    def package(self):
        self.copy("*.hpp")

    def package_id(self):
        self.info.header_only()
