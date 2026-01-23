cmake_minimum_required(VERSION 3.15)
project(SinistarModern VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Options
option(BUILD_TESTS "Build test suite" ON)

# Platform-specific settings
if(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "Minimum macOS deployment version")
    set(MACOSX_BUNDLE_ICON_FILE sinistar.icns)
    set(APP_ICON ${CMAKE_SOURCE_DIR}/assets/sinistar.icns)
    if(EXISTS ${APP_ICON})
        set_source_files_properties(${APP_ICON} PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources")
    endif()
elseif(WIN32)
    # Windows-specific settings
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

# Find SDL2
# SDL2 can be installed via:
#   macOS: brew install sdl2 sdl2_mixer sdl2_image
#   Windows: Download from https://www.libsdl.org/ or use vcpkg
find_package(SDL2 REQUIRED)

# SDL2_mixer and SDL2_image (optional for now, will be required later)
find_package(SDL2_mixer QUIET)
find_package(SDL2_image QUIET)

# Source files
set(CORE_SOURCES
    src/core/GameEngine.cpp
    src/core/TaskManager.cpp
    src/core/GameState.cpp
    src/core/ScoreManager.cpp
    src/core/Timer.cpp
    src/core/Math.cpp
)

set(ENTITY_SOURCES
    src/entities/GameObject.cpp
    src/entities/Player.cpp
    src/entities/Sinibomb.cpp
    src/entities/Warrior.cpp
)

set(SYSTEM_SOURCES
    src/systems/RenderSystem.cpp
    src/systems/PhysicsSystem.cpp
)

set(INPUT_SOURCES
    src/input/InputSystem.cpp
)

set(GRAPHICS_SOURCES
    src/graphics/Color.cpp
    src/graphics/TextRenderer.cpp
    src/graphics/Particle.cpp
)

set(AUDIO_SOURCES
    src/audio/AudioManager.cpp
)

set(MAIN_SOURCE
    src/main.cpp
)

# All sources
set(ALL_SOURCES
    ${MAIN_SOURCE}
    ${CORE_SOURCES}
    ${ENTITY_SOURCES}
    ${SYSTEM_SOURCES}
    ${INPUT_SOURCES}
    ${GRAPHICS_SOURCES}
    ${AUDIO_SOURCES}
)

# Header files (for IDE)
file(GLOB_RECURSE HEADERS "src/*.h" "src/*.hpp")

# Create executable
if(APPLE)
    if(EXISTS ${APP_ICON})
        add_executable(Sinistar MACOSX_BUNDLE ${ALL_SOURCES} ${HEADERS} ${APP_ICON})
    else()
        add_executable(Sinistar MACOSX_BUNDLE ${ALL_SOURCES} ${HEADERS})
    endif()
    set_target_properties(Sinistar PROPERTIES
        MACOSX_BUNDLE_BUNDLE_NAME "Sinistar"
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.sinistar.game"
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION}
    )
elseif(WIN32)
    add_executable(Sinistar WIN32 ${ALL_SOURCES} ${HEADERS})
else()
    add_executable(Sinistar ${ALL_SOURCES} ${HEADERS})
endif()

# Include directories
target_include_directories(Sinistar PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${SDL2_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(Sinistar PRIVATE
    SDL2::SDL2
)

# Link SDL2main for Windows/macOS
if(TARGET SDL2::SDL2main)
    target_link_libraries(Sinistar PRIVATE SDL2::SDL2main)
endif()

# Optional libraries
if(SDL2_mixer_FOUND)
    target_link_libraries(Sinistar PRIVATE SDL2_mixer::SDL2_mixer)
    target_compile_definitions(Sinistar PRIVATE HAVE_SDL2_MIXER)
endif()

if(SDL2_image_FOUND)
    target_link_libraries(Sinistar PRIVATE SDL2_image::SDL2_image)
    target_compile_definitions(Sinistar PRIVATE HAVE_SDL2_IMAGE)
endif()

# Compiler warnings
if(MSVC)
    target_compile_options(Sinistar PRIVATE /W4)
else()
    target_compile_options(Sinistar PRIVATE -Wall -Wextra -pedantic)
endif()

# Copy assets to build directory (if they exist)
# Create assets directory first - actual assets will be added in future phases
add_custom_command(TARGET Sinistar POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:Sinistar>/assets
    COMMENT "Preparing assets directory"
)

# Installation
install(TARGETS Sinistar
    BUNDLE DESTINATION .
    RUNTIME DESTINATION bin
)
install(DIRECTORY assets DESTINATION share/sinistar)

# Tests
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Print configuration summary
message(STATUS "")
message(STATUS "Sinistar Modern Configuration Summary:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  SDL2: ${SDL2_VERSION}")
if(SDL2_mixer_FOUND)
    message(STATUS "  SDL2_mixer: Found")
else()
    message(STATUS "  SDL2_mixer: Not found (audio will be limited)")
endif()
if(SDL2_image_FOUND)
    message(STATUS "  SDL2_image: Found")
else()
    message(STATUS "  SDL2_image: Not found (will use SDL2 image loading)")
endif()
message(STATUS "")
