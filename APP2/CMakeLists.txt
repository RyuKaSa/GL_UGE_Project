# Collect header and source files (excluding Main.cpp as it's used for the main executable)
file(GLOB HEADER_FILES *.hpp)
file(GLOB SRC_FILES *.cpp)
list(REMOVE_ITEM SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

# Define the main executable name
set(OUTPUT main_executable)
message(STATUS "Configuring executable ${OUTPUT}")

# Create the executable with Main.cpp and all other source and header files
add_executable(${OUTPUT} main.cpp ${SRC_FILES} ${HEADER_FILES})

# Link libraries and set include directories
target_link_libraries(${OUTPUT} PRIVATE glimac)
target_include_directories(${OUTPUT} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${OUTPUT} PRIVATE ${CMAKE_SOURCE_DIR}/glimac/include)
target_include_directories(${OUTPUT} PRIVATE ${CMAKE_SOURCE_DIR}/glimac/third-party/glad/include)

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

# Include custom scripts for copying files
include("${CMAKE_SOURCE_DIR}/CMakeUtils/files_and_folders.cmake")
Cool__target_copy_folder(${OUTPUT} ../assets)
Cool__target_copy_folder(${OUTPUT} shaders)