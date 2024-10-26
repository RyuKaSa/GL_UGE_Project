file(GLOB HEADER_FILES *.hpp)
file(GLOB EXT_SRC_FILES [^e][^x]*.cpp)
file(GLOB EXE_SRC_FILES ex[^\#]*.cpp)
#file(GLOB_RECURSE SHADER_FILES *.glsl)
list(REMOVE_ITEM EXT_SRC_FILES ${EXE_SRC_FILES})

foreach(EXE_SRC_FILE ${EXE_SRC_FILES})
	get_filename_component(FILE ${EXE_SRC_FILE} NAME_WE)
	get_filename_component(DIR ${CMAKE_CURRENT_SOURCE_DIR} NAME)

    set(OUTPUT ${DIR}_${FILE})
    message(STATUS "Configuring executable ${OUTPUT}")
	set(SRC_FILES ${EXT_SRC_FILES} ${EXE_SRC_FILE})
	add_executable(${OUTPUT} ${SRC_FILES} ${HEADER_FILES})

    target_link_libraries(${OUTPUT} glimac)
    target_include_directories(${OUTPUT} PUBLIC glimac)
    target_include_directories(${OUTPUT} PUBLIC glimac/third-party/glad/include)

    set_target_properties(${OUTPUT} PROPERTIES
		CXX_STANDARD 11
		CXX_STANDARD_REQUIRED YES
		CXX_EXTENSIONS NO
	)
	set_target_properties(${OUTPUT} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
	if (MSVC)
		target_compile_options(${OUTPUT} PRIVATE /W3)
	else()
		target_compile_options(${OUTPUT} PRIVATE -Wall -Wextra -Wpedantic -pedantic-errors)
	endif()
endforeach()
include("../CMakeUtils/files_and_folders.cmake")
Cool__target_copy_folder(${OUTPUT} ../assets)
Cool__target_copy_folder(${OUTPUT} shaders)

#file(COPY shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
