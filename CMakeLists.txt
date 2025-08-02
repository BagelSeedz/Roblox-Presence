cmake_minimum_required(VERSION 3.10)
project(Roblox-Presence)

include_directories(${CMAKE_SOURCE_DIR}/third_party)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add executable with platform-specific resource file
if(WIN32)
    add_executable(Roblox-Presence main.cpp resource.rc)
else()
    add_executable(Roblox-Presence main.cpp)
endif()

# Define Social SDK variables
set(DISCORD_SDK_ROOT "${CMAKE_SOURCE_DIR}/lib/discord_social_sdk")
set(DISCORD_SDK_LIB_DIR "${DISCORD_SDK_ROOT}/lib/release")
set(DISCORD_SDK_BIN_DIR "${DISCORD_SDK_ROOT}/bin/release")
set(DISCORD_SDK_INCLUDE_DIR "${DISCORD_SDK_ROOT}/include")

target_include_directories(Roblox-Presence PRIVATE ${DISCORD_SDK_INCLUDE_DIR})

# Link Discord SDK depending on platform
if(WIN32)
    set(DISCORD_LIB_PATH "${DISCORD_SDK_LIB_DIR}/discord_partner_sdk.lib")
    set(DISCORD_SHARED_LIB "${DISCORD_SDK_BIN_DIR}/discord_partner_sdk.dll")
    set_target_properties(Roblox-Presence PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
elseif(APPLE)
    set(DISCORD_LIB_PATH "${DISCORD_SDK_LIB_DIR}/libdiscord_partner_sdk.dylib")
    set(DISCORD_SHARED_LIB "${DISCORD_SDK_LIB_DIR}/libdiscord_partner_sdk.dylib")
else()
    set(DISCORD_LIB_PATH "${DISCORD_SDK_LIB_DIR}/libdiscord_partner_sdk.so")
    set(DISCORD_SHARED_LIB "${DISCORD_SDK_LIB_DIR}/libdiscord_partner_sdk.so")
endif()

target_link_libraries(Roblox-Presence PRIVATE ${DISCORD_LIB_PATH})

# Copy DLL to output folder
add_custom_command(TARGET Roblox-Presence POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${DISCORD_SHARED_LIB}"
    $<TARGET_FILE_DIR:Roblox-Presence>
)

# Add RPATH settings for non-Windows platforms
if(UNIX)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    set(CMAKE_INSTALL_RPATH "$ORIGIN")
    if(APPLE)
        set(CMAKE_INSTALL_RPATH "@executable_path")
    endif()
endif()
