cmake_minimum_required(VERSION 3.15)
project(VRO)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Path to OpenVR SDK (adjust if needed)
set(OPENVR_DIR ${CMAKE_SOURCE_DIR}/external/openvr)

# Include all source and header files
file(GLOB_RECURSE SRC_FILES "src/**/*.cpp")
file(GLOB_RECURSE HEADER_FILES "src/**/*.h")
set(MAIN_CPP "src/main.cpp")

# Include directories
include_directories(VRO PRIVATE
    ${OPENVR_DIR}/headers
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_SOURCE_DIR}/src/render
    ${CMAKE_SOURCE_DIR}/src/osc
    ${CMAKE_SOURCE_DIR}/src/UI
)

# Add executable
add_executable(VRO ${MAIN_CPP} ${SRC_FILES} ${HEADER_FILES})

# Link OpenVR and DirectX 11 libraries
target_link_libraries(VRO
    "${OPENVR_DIR}/lib/win32/openvr_api.lib"  # full path ensures linker finds it
    d3d11
    dxgi
    d3dcompiler
    user32
    gdi32
    dxguid
)

set_target_properties(VRO PROPERTIES
    LINK_FLAGS "/SUBSYSTEM:CONSOLE"
)