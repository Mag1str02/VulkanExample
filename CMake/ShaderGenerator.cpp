#include <chrono>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <process.hpp>
#include <thread>
#include <unordered_map>
#include <vector>

using namespace TinyProcessLib;
using namespace std::chrono_literals;

namespace fs = std::filesystem;

enum class ShaderStage {
    Unknown = 0,
    Vertex,
    Fragment,
};
static const std::vector<ShaderStage> s_AllShaderStages = {ShaderStage::Vertex, ShaderStage::Fragment};

using ShaderGroup   = std::unordered_map<std::string, std::string>;
using ShaderStorage = std::unordered_map<ShaderStage, ShaderGroup>;

std::string ReadFile(std::ifstream& stream) {
    int length;
    stream.seekg(0, std::ios::end);
    length = stream.tellg();
    std::string res(length, ' ');
    stream.seekg(0, std::ios::beg);
    stream.read(res.data(), length);
    return res;
}

ShaderStage GetShaderStage(const fs::path& path) {
    if (path.extension() == ".vert" || path.extension() == ".vs") {
        return ShaderStage::Vertex;
    }
    if (path.extension() == ".frag" || path.extension() == ".fs") {
        return ShaderStage::Fragment;
    }
    return ShaderStage::Unknown;
}
std::string GetShaderStageString(ShaderStage stage) {
    switch (stage) {
        case ShaderStage::Vertex: return "Vertex";
        case ShaderStage::Fragment: return "Fragment";
        default: return "Unknown";
    }
}

std::string Ident(uint32_t count);

class ShaderGenerator {
public:
    ShaderGenerator(int argc, char** argv) {
        if (argc < 5) {
            throw std::runtime_error(std::format("Should be 5 or more arguments, but only {} were provided", argc));
        }
        m_SpirvCompiler   = argv[1];
        m_HeaderFilepath  = argv[2];
        m_CppFilepath     = argv[3];
        m_ShaderDirectory = argv[4];
        for (int i = 5; i < argc; ++i) {
            m_Shaders.push_back(argv[i]);
            std::stringstream command;
            command << m_SpirvCompiler;
        }
    }
    void Generate() {
        Compile();
        GenerateHeader();
        GenerateCpp();
    }

private:
    void GenerateHeader() {
        std::ofstream headerFile(m_HeaderFilepath);
        if (!headerFile.is_open()) {
            throw std::runtime_error(std::format("Failed to create generated header file: {}", m_HeaderFilepath.string()));
        }
        headerFile << "#pragma once" << std::endl;
        headerFile << "#include <vector>" << std::endl;
        headerFile << "#include <string>" << std::endl;
        headerFile << "#include <optional>" << std::endl;
        headerFile << "#include <cstdint>" << std::endl;
        headerFile << std::endl;
        for (const auto& stage : s_AllShaderStages) {
            headerFile << "std::optional<const std::vector<uint32_t>*> Get" << GetShaderStageString(stage) << "ShaderCode(const std::string& shaderName);"
                       << std::endl;
        }
    }
    void GenerateCpp() {
        std::ofstream cppFile(m_CppFilepath);
        if (!cppFile.is_open()) {
            throw std::runtime_error(std::format("Failed to create generated cpp file: {}", m_CppFilepath.string()));
        }
        cppFile << "#include \"" << m_HeaderFilepath.filename().string() << "\"" << std::endl;
        cppFile << "#include <unordered_map>" << std::endl;
        cppFile << std::endl;
        for (const auto& stage : s_AllShaderStages) {
            cppFile << "std::optional<const std::vector<uint32_t>*> Get" << GetShaderStageString(stage) << "ShaderCode(const std::string& shaderName) {"
                    << std::endl;
            cppFile << "static const std::unordered_map<std::string, std::vector<uint32_t>> Shaders = {" << std::endl;
            for (const auto& [name, code] : m_ShaderStorage[stage]) {
                cppFile << "{\"" << name << "\", " << code << "}," << std::endl;
            }
            cppFile << "};" << std::endl;
            cppFile << "if (auto it = Shaders.find(shaderName); it != Shaders.end()) {" << std::endl;
            cppFile << "    return &(it->second);" << std::endl;
            cppFile << "}" << std::endl;
            cppFile << "return {};" << std::endl;
            cppFile << "}" << std::endl;
        }
    }
    void Compile() {
        for (const auto& shader : m_Shaders) {
            auto type = GetShaderStage(shader);
            if (type == ShaderStage::Unknown) {
                throw std::runtime_error(std::format("Unknown shader extension: {}", shader.extension().string()));
            }
            const char*    tmpFileName    = std::tmpnam(nullptr);
            const fs::path shaderFilepath = m_ShaderDirectory / tmpFileName;

            std::ofstream outputFile(shaderFilepath);
            if (!outputFile.is_open()) {
                throw std::runtime_error(std::format("Failed to create tmp file for shader compilation: {}", shaderFilepath.string()));
            }
            std::ifstream inputFile(shaderFilepath);
            if (!inputFile.is_open()) {
                throw std::runtime_error(std::format("Failed to open tmp file for shader compilation: {}", shaderFilepath.string()));
            }
            std::vector<std::string> arguments = {m_SpirvCompiler.string(), "-mfmt=c", (m_ShaderDirectory / shader).string(), "-o", shaderFilepath.string()};

            Process process1a(arguments);
            auto    exit_status = process1a.get_exit_status();
            if (exit_status != 0) {
                throw std::runtime_error(std::format("Failed to compile shader: {}", shader.string()));
            }
            std::string shaderName = shader.stem().string();
            if (m_ShaderStorage[type].contains(shaderName)) {
                throw std::runtime_error(std::format("Shader with name {} already exists", shaderName));
            }
            m_ShaderStorage[type][shaderName] = ReadFile(inputFile);
        }
    }

    fs::path              m_SpirvCompiler;
    fs::path              m_HeaderFilepath;
    fs::path              m_CppFilepath;
    fs::path              m_ShaderDirectory;
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