#include <filesystem>
#include <format>
#include <iostream>
#include <unordered_map>
#include <vector>

namespace fs = std::filesystem;

enum class ShaderStage {
    Unknown = 0,
    Vertex,
    Fragment,
};

ShaderStage GetShaderType(const fs::path& path) {
    if (path.extension() == "vert" || path.extension() == "vs") {
        return ShaderStage::Vertex;
    }
    if (path.extension() == "frag" || path.extension() == "fs") {
        return ShaderStage::Fragment;
    }
    return ShaderStage::Unknown;
}

class ShaderGenerator {
public:
    using ShaderGroup   = std::vector<const std::vector<char>>;
    using ShaderStorage = std::unordered_map<ShaderStage, ShaderGroup>;

    ShaderGenerator(int argc, char** argv) {
        if (argc < 3) {
            throw std::runtime_error(std::format("Should be 3 or more arguments, but only {} were provided", argc));
        }
        m_SpirvCompiler  = argv[1];
        m_OutputFilePath = argv[2];
        for (int i = 3; i < argc; ++i) {
            m_Shaders.push_back(argv[i]);
            std::stringstream command;
            command << m_SpirvCompiler;
        }
    }
    void Generate() { Compile(); }

private:
    void Compile() {
        for (const auto& shader : m_Shaders) {
            auto type = GetShaderType(shader);
            if (type == ShaderStage::Unknown) {
                throw std::runtime_error(std::format("Unknown shader extension: {}", shader.extension()));
            }
        }
    }

    fs::path              m_SpirvCompiler;
    fs::path              m_OutputFilePath;
    std::vector<fs::path> m_Shaders;

    ShaderStorage m_ShaderStorage;
};

int main(int argc, char** argv) {
    try {
        ShaderGenerator generator(argc, argv);
        generator.Generate();
    } catch (const std::exception& ex) {
        std::cerr << "Failed to generate shaders header due to: " << ex.what() << std::endl;
        return 1;
    }
    return 0;
}