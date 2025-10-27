cmake_minimum_required (VERSION 3.11)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# Version Information -----------------------------------------------------------------------------
project ("Voxels")
set (VERSION_MAJOR 0)
set (VERSION_MINOR 1)
set (VERSION_PATCH 1)
set (VERSION "${VERSION_MAJOR}.${VERSION_MINOR}")
set (VERSION "${VERSION}.${VERSION_PATCH}")
message ("Version: ${VERSION}")


# Configure Source & Binary Directories -----------------------------------------------------------
set (PROJECT_ROOT			"${PROJECT_SOURCE_DIR}")
set (PROJECT_EXTERNAL_DIR	"${PROJECT_SOURCE_DIR}/3rdparty")
set (PROJECT_RESOURCES_DIR	"${PROJECT_SOURCE_DIR}/res")
set (PROJECT_SOURCE_DIR		"${PROJECT_SOURCE_DIR}/src")
set (PROJECT_BINARY_DIR		"${PROJECT_BINARY_DIR}/bin")
message ("Source path: ${PROJECT_SOURCE_DIR}")
message ("External path: ${PROJECT_EXTERNAL_DIR}")
message ("Resources path: ${PROJECT_RESOURCES_DIR}")
message ("Binary path: ${PROJECT_BINARY_DIR}")



include_directories ("include" "3rdparty")


add_executable (Voxels "${PROJECT_SOURCE_DIR}/unity/voxel_unity.cpp" )



if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Voxels PROPERTY CXX_STANDARD 23)
endif()


include(FetchContent)

# glm
FetchContent_Declare(
	glm
	GIT_REPOSITORY	https://github.com/g-truc/glm
	GIT_TAG			0af55ccecd98d4e5a8d1fad7de25ba429d60e863
)

FetchContent_MakeAvailable(glm)

target_link_libraries(Voxels PRIVATE glm::glm)

# glfw

FetchContent_Declare(
	glfw
	GIT_REPOSITORY	https://github.com/glfw/glfw
	GIT_TAG			7b6aead9fb88b3623e3b3725ebb42670cbe4c579
)

FetchContent_MakeAvailable(glfw)

target_link_libraries(Voxels PRIVATE glfw)




