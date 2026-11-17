cmake_minimum_required(VERSION 3.20)
project(3DFunctCalc LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(3DFunctCalc 
    main.cpp
    core/connection/soketManager.cpp
)

set(Vulkan_ROOT "$ENV{VULKAN_SDK}")
find_package(Vulkan REQUIRED)

target_link_libraries(3DFunctCalc PRIVATE 
    Vulkan::Vulkan
    ws2_32
)

if(MSVC)
    target_link_options(3DFunctCalc PRIVATE "/SUBSYSTEM:CONSOLE")
    #target_link_options(3DFunctCalc PRIVATE "/SUBSYSTEM:WINDOWS")

elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    #target_link_options(3DFunctCalc PRIVATE "-mwindows")
    if(MINGW)
        target_link_options(3DFunctCalc PRIVATE "-municode")
    endif()
endif()