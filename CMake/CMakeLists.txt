add_executable(ShaderGenerator
    ShaderGenerator.cpp
)
target_link_libraries(ShaderGenerator PUBLIC
    tiny-process-library 
)
target_include_directories(ShaderGenerator PUBLIC
    ${PROJECT_SOURCE_DIR}/Dependencies/TPL
)

set_target_properties(ShaderGenerator
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)