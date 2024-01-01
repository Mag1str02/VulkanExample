macro( target_shaders )

    set( _OPTIONS_ARGS )
    set( _ONE_VALUE_ARGS TARGET PATH)
    set( _MULTI_VALUE_ARGS SHADERS )

    cmake_parse_arguments( _TARGET_SHADERS "${_OPTIONS_ARGS}" "${_ONE_VALUE_ARGS}" "${_MULTI_VALUE_ARGS}" ${ARGN} )

    if (NOT _TARGET_SHADERS_SHADERS)
        message( FATAL_ERROR "No SHADERS in target_shaders")
    endif()
    if (NOT _TARGET_SHADERS_TARGET)
        message( FATAL_ERROR "No TARGET in target_shaders")
    endif()
    if (NOT _TARGET_SHADERS_PATH)
        message( FATAL_ERROR "No PATH in target_shaders")
    endif()

    foreach(SHADER ${_TARGET_SHADERS_SHADERS})
        set(_CUSTOM_TARGET_NAME SHADER_COMPILATION_${TARGET_SHADERS_TARGET}_${SHADER})
        set(_DIRECTORY_TARGET_NAME DIRECTORY_CREATE_${TARGET_SHADERS_TARGET}_${SHADER})
        add_custom_target(${_CUSTOM_TARGET_NAME}
            COMMAND
                glslc ${PROJECT_SOURCE_DIR}/${_TARGET_SHADERS_PATH}/${SHADER} -o ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${_TARGET_SHADERS_PATH}/${SHADER}.spv 
            DEPENDS
                ${PROJECT_SOURCE_DIR}/${_TARGET_SHADERS_PATH}/${SHADER}
            BYPRODUCTS
                ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${_TARGET_SHADERS_PATH}/${SHADER}.spv
        )
        add_custom_target(${_DIRECTORY_TARGET_NAME}
            COMMAND
                ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${_TARGET_SHADERS_PATH}
        )
        add_dependencies(${_CUSTOM_TARGET_NAME} ${_DIRECTORY_TARGET_NAME})
        add_dependencies(${_TARGET_SHADERS_TARGET} ${_CUSTOM_TARGET_NAME})
    endforeach()
    
endmacro()
