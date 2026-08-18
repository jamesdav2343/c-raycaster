from conan import ConanFile
from conan.tools.cmake import CMake, CMakeDeps, CMakeToolchain, cmake_layout

class RaycasterConan(ConanFile):
    name = "raycaster"
    version = "0.1.0"

    settings = "os", "compiler", "build_type", "arch"

    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "enable_tests": [True, False],
    }
    default_options = {
        "shared": False,
        "fPIC": True,
        "enable_tests": False,
    }

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def configure(self):
        if self.options.shared:
            self.options.rm_safe("fPIC")

    def layout(self):
        cmake_layout(self)

    def requirements(self):
        self.requires("sdl/3.2.20")
        self.requires("sdl_image/3.2.4")
        self.requires("sdl_ttf/3.2.2")
        self.requires("flecs/4.1.1")
        self.requires("glib/2.85.3")
        self.requires("cjson/1.7.19")

    def build_requirements(self):
        self.tool_requires("cmake/[>=3.25]", visible=True)

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()

        tc = CMakeToolchain(self)
        tc.variables["BUILD_TESTING"] = self.options.enable_tests
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()