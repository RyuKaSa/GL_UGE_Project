cmake_minimum_required(VERSION 3.8)
cmake_policy(SET CMP0072 NEW)

project(TD_GL_LEVEL2)

# Set the folder where the executables are created
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

# Include glimac
add_subdirectory(glimac)

file(GLOB TP_DIRECTORIES "TP*")

foreach(TP ${TP_DIRECTORIES})
    message(WARNING "Configuring ${TP}")
    add_subdirectory(${TP})
	# get_filename_component(DIRN ${TP} NAME)
    # add_custom_target(${DIRN} COMMAND cd ${CMAKE_CURRENT_BINARY_DIR}/${DIRN} COMMAND make
    #             WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${DIRN})
endforeach()

# Create a target for each TP
# function(setup_tp TP_NUMBER)    
#     set(TARGET_NAME ${TP_NUMBER}_exe)
#     add_executable(${TARGET_NAME})
#     target_compile_features(${TARGET_NAME} PRIVATE cxx_std_11)

#     if (MSVC)
#         target_compile_options(${TARGET_NAME} PRIVATE /WX /W3)
#     else()
#         target_compile_options(${TARGET_NAME} PRIVATE -Werror -W -Wall -Wextra -Wpedantic -pedantic-errors)
#     endif()
    
#     target_include_directories(${TARGET_NAME} PRIVATE ${TP_NUMBER})
#     file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS ${TP_NUMBER}/*)
#     target_sources(${TARGET_NAME} PRIVATE ${MY_SOURCES})

#     target_link_libraries(${TARGET_NAME} glimac)

#     include("CMakeUtils/files_and_folders.cmake")
#     Cool__target_copy_folder(${TARGET_NAME} assets)
#     Cool__target_copy_folder(${TARGET_NAME} ${TP_NUMBER}/shaders)
# endfunction(setup_tp)

# setup_tp(TP1)
# setup_tp(TP2)
# setup_tp(TP3)
# setup_tp(TP4)
