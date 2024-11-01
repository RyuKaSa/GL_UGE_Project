cmake_minimum_required(VERSION 3.8)
project(glimac)

# Create the 'glimac' library
add_library(glimac)

# Collect source files
file(GLOB_RECURSE GLIMAC_SOURCES CONFIGURE_DEPENDS src/*.cpp)
target_sources(glimac PRIVATE ${GLIMAC_SOURCES})

# Include directories
target_include_directories(glimac PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# --- Add GLFW ---
add_subdirectory(third-party/glfw)
target_link_libraries(glimac PUBLIC glfw)

# --- Add glad ---
add_library(glad third-party/glad/src/glad.c)
target_include_directories(glad PUBLIC third-party/glad/include)
target_link_libraries(glimac PUBLIC glad)

# --- Add glm ---
add_subdirectory(third-party/glm)
target_link_libraries(glimac PUBLIC glm)

# Find SDL2
find_package(SDL2 REQUIRED)
if(SDL2_FOUND)
    target_include_directories(glimac PUBLIC ${SDL2_INCLUDE_DIRS})
    target_link_libraries(glimac PUBLIC ${SDL2_LIBRARIES})
else()
    message(FATAL_ERROR "SDL2 not found")
endif()