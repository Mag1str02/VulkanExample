add_library(VulkanHelpers 
    Application.cpp
    Assert.cpp
)

target_link_libraries(VulkanHelpers 
    PUBLIC Dependencies
)

target_include_directories(VulkanHelpers 
    PUBLIC ../.
)