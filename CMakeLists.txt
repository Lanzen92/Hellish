cmake_minimum_required(VERSION 3.26)
project(Practice LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(DLL_NAME ${PROJECT_NAME}_game)
set(DIR_ASSETS_ORIGIN ${CMAKE_SOURCE_DIR}/assets)
set(DIR_ASSETS_DESTINATION $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets)
set(SHARED_LIB_NAME ${PROJECT_NAME}_common)
set(WINDOWS_LIBRARIES winmm)

#Flag EXE specific files
set(EXE_EXCLUSIVE ${CMAKE_SOURCE_DIR}/src/main.cpp
)
#Flag shared files (For both EXE and DLLs)
set(SHARED_SOURCES 
	${CMAKE_SOURCE_DIR}/src/main.cpp
	${CMAKE_SOURCE_DIR}/src/arena.cpp
	${CMAKE_SOURCE_DIR}/src/input.cpp

)

set(IMGUI
	${CMAKE_SOURCE_DIR}/src_external/imgui/imgui.cpp
	${CMAKE_SOURCE_DIR}/src_external/imgui/imgui_draw.cpp
	${CMAKE_SOURCE_DIR}/src_external/imgui/imgui_tables.cpp
	${CMAKE_SOURCE_DIR}/src_external/imgui/imgui_widgets.cpp
	${CMAKE_SOURCE_DIR}/src_external/imgui/imgui_impl_sdlrenderer3.cpp
	${CMAKE_SOURCE_DIR}/src_external/imgui/imgui_impl_sdl3.cpp
)



#Shared static dll
add_library(${SHARED_LIB_NAME} STATIC ${SHARED_SOURCES})
target_include_directories(${SHARED_LIB_NAME} PRIVATE include src_external)

# Automatically find all .cpp files in src/
# file(GLOB_RECURSE SOURCES "src/*.cpp")

# Create the file references
file(GLOB_RECURSE LIB_FILES "${CMAKE_SOURCE_DIR}/lib/*.lib")
file(GLOB_RECURSE DLL_FILES "${CMAKE_SOURCE_DIR}/lib/*.dll")
file(GLOB_RECURSE DLL_EXCLUSIVE "src/*.cpp")
list(REMOVE_ITEM DLL_EXCLUSIVE ${EXE_EXCLUSIVE})

# Create executable
add_executable(${PROJECT_NAME} ${EXE_EXCLUSIVE})
target_include_directories(${PROJECT_NAME} PRIVATE include src_external)
target_link_libraries(${PROJECT_NAME} PRIVATE 
	${LIB_FILES}
	${WINDOWS_LIBRARIES}
	${SHARED_LIB_NAME}
)

# Add include directory and ImGui to the shared library
add_library(${DLL_NAME} SHARED ${DLL_EXCLUSIVE} ${IMGUI})
target_include_directories(${DLL_NAME} PRIVATE include src_external)
target_link_libraries(${DLL_NAME} PRIVATE 
	${LIB_FILES}
	${SHARED_LIB_NAME}
)

# Automatically copy all .dll files to executable directory
file(GLOB_RECURSE DLL_FILES "${CMAKE_SOURCE_DIR}/lib/*.dll")
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
		${DLL_FILES}
		"$<TARGET_FILE_DIR:${PROJECT_NAME}>"
)

#Post-Build. Copy assets to build folder.
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD 
	COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different ${DIR_ASSETS_ORIGIN} ${DIR_ASSETS_DESTINATION} VERBATIM
)
