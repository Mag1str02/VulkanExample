find_program (SPIRV_COMPILER_PATH ${SPIRV_COMPILER})
if (${SPIRV_COMPILER_PATH} STREQUAL "SPIRV_COMPILER_PATH-NOTFOUND")
    message(FATAL_ERROR "Failed to find SPIRV compiler: ${SPIRV_COMPILER_PATH}")
else()
    message(STATUS "Found SPIRV compiler: ${SPIRV_COMPILER_PATH}")
endif()

macro( target_shaders )
    set( _OPTIONS_ARGS )
    set( _ONE_VALUE_ARGS TARGET)
    set( _MULTI_VALUE_ARGS SHADERS )

    cmake_parse_arguments( _TARGET_SHADERS "${_OPTIONS_ARGS}" "${_ONE_VALUE_ARGS}" "${_MULTI_VALUE_ARGS}" ${ARGN} )

    if (NOT _TARGET_SHADERS_SHADERS)
        message( FATAL_ERROR "No SHADERS in target_shaders")
    endif()
    if (NOT _TARGET_SHADERS_TARGET)
        message( FATAL_ERROR "No TARGET in target_shaders")
    endif()

    set(_GENERATION_TARGET_NAME SHADER_GENERATION_FOR_${_TARGET_SHADERS_TARGET})
    set(_GENERATED_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/shaders_generated.h)
    set(_GENERATED_CPP ${CMAKE_CURRENT_SOURCE_DIR}/shaders_generated.cpp)
    add_custom_target(${_GENERATION_TARGET_NAME}
            COMMAND
                ShaderGenerator ${SPIRV_COMPILER_PATH} ${_GENERATED_HEADER} ${_GENERATED_CPP} ${CMAKE_CURRENT_SOURCE_DIR} ${_TARGET_SHADERS_SHADERS}
            DEPENDS
                ${_TARGET_SHADERS_SHADERS}
            BYPRODUCTS
                ${_GENERATED_HEADER} ${_GENERATED_CPP}
    )
    add_dependencies(${_GENERATION_TARGET_NAME} ShaderGenerator)
    add_dependencies(${_TARGET_SHADERS_TARGET} ${_GENERATION_TARGET_NAME})
    target_sources(${_TARGET_SHADERS_TARGET} PUBLIC
        ${_GENERATED_CPP}
    )

endmacro()
