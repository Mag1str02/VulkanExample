add_executable(ShaderGenerator
    ShaderGenerator.cpp
)

set_target_properties(ShaderGenerator
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)