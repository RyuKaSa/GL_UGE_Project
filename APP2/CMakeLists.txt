# Collect header and source files
file(GLOB HEADER_FILES *.hpp)
file(GLOB SRC_FILES *.cpp)

# Iterate over each source file to create an executable
foreach(SRC_FILE ${SRC_FILES})
    # Get the filename (without extension) and directory name
    get_filename_component(FILE ${SRC_FILE} NAME_WE)
    get_filename_component(DIR ${CMAKE_CURRENT_SOURCE_DIR} NAME)

    # Define the output executable name
    set(OUTPUT ${DIR}_${FILE})
    message(STATUS "Configuring executable ${OUTPUT}")

    # Create the executable
    add_executable(${OUTPUT} ${SRC_FILE} ${HEADER_FILES})

    # Link libraries and set include directories
    target_link_libraries(${OUTPUT} glimac)
    target_include_directories(${OUTPUT} PUBLIC glimac)
    target_include_directories(${OUTPUT} PUBLIC glimac/third-party/glad/include)

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
endforeach()

# Include custom scripts for copying files
include("../CMakeUtils/files_and_folders.cmake")
Cool__target_copy_folder(${OUTPUT} ../assets)
Cool__target_copy_folder(${OUTPUT} shaders)