# Collect all header and source files (including subdirectories)
file(GLOB_RECURSE HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)
file(GLOB_RECURSE SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

# Remove main.cpp from the collected source files list
list(REMOVE_ITEM SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

# Define the main executable name
set(OUTPUT APP3_executable)
message(STATUS "Configuring executable ${OUTPUT}")

# Debug output for collected files
message(STATUS "Source files: ${SRC_FILES}")
message(STATUS "Header files: ${HEADER_FILES}")

# Create the executable with Main.cpp and other source files
add_executable(${OUTPUT} main.cpp ${SRC_FILES})

# Link libraries and set include directories
target_link_libraries(${OUTPUT} PRIVATE glimac)
target_include_directories(${OUTPUT} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${OUTPUT} PRIVATE ${CMAKE_SOURCE_DIR}/glimac/include)
target_include_directories(${OUTPUT} PRIVATE ${CMAKE_SOURCE_DIR}/glimac/third-party/glad/include)

# Add OpenGL and GLU libraries
find_package(OpenGL REQUIRED)
find_library(GLU_LIBRARY GLU)

if (GLU_LIBRARY)
    target_link_libraries(${OUTPUT} PRIVATE ${OPENGL_LIBRARIES} ${GLU_LIBRARY})
else()
    message(FATAL_ERROR "GLU library not found!")
endif()

# Add SDL library
find_package(SDL REQUIRED) # SDL 1.2
include_directories(${SDL_INCLUDE_DIR})

target_link_libraries(${OUTPUT} 
    PRIVATE 
        glimac
        ${SDL_LIBRARY}
        dl
        OpenGL::GL
        ${GLU_LIBRARY}
)

# Set properties for the target
set_target_properties(${OUTPUT} PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)
set_target_properties(${OUTPUT} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Add compiler options for different platforms
if (MSVC)
    target_compile_options(${OUTPUT} PRIVATE /W3)
else()
    target_compile_options(${OUTPUT} PRIVATE -Wall -Wextra -Wpedantic -pedantic-errors)
endif()

# Include custom scripts for copying files (if needed)
include("${CMAKE_SOURCE_DIR}/CMakeUtils/files_and_folders.cmake")
Cool__target_copy_folder(${OUTPUT} ../assets)
Cool__target_copy_folder(${OUTPUT} shaders)
