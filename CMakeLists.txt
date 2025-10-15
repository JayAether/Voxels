cmake_minimum_required (VERSION 3.11)

#if (POLICY CMP0141)
#  cmake_policy(SET CMP0141 NEW)
#  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
#endif()

# Version Information -----------------------------------------------------------------------------
project ("Voxels")
set (VERSION_MAJOR 0)
set (VERSION_MINOR 1)
set (VERSION_PATCH 0)
set (VERSION "${VERSION_MAJOR}.${VERSION_MINOR}")
set (VERSION "${VERSION}.${VERSION_PATCH}")
message ("Version: ${VERSION}")


# Configure Source & Binary Directories -----------------------------------------------------------
set (PROJECT_ROOT "${PROJECT_SOURCE_DIR}")
set (PROJECT_SOURCE_DIR "${PROJECT_SOURCE_DIR}/src")
set (PROJECT_BINARY_DIR "${PROJECT_BINARY_DIR}/bin")
message ("Source path: ${PROJECT_SOURCE_DIR}")
message ("Binary path: ${PROJECT_BINARY_DIR}")



#include_directories("
#	${PROJECT_SOURCE_DIR}
#	${PROJECT_BINARY_DIR}
#	${PROJECT_ROOT}
#	")


include_directories ("include" "3rdparty")

# Add source to this project's executable.
#file(GLOB_RECURSE SRC_FILES "src/*.cpp")
#file(GLOB_RECURSE SRC_FILES "${PROJECT_SOURCE_DIR}/src/*.c")
#file(GLOB_RECURSE SRC_FILES "src/*.h")
#file(GLOB_RECURSE SRC_FILES "src/*.c")

add_executable (Voxels "src/main.cpp" "src/glad.c" "${PROJECT_SOURCE_DIR}/engine/Engine.cpp" 
	"${PROJECT_SOURCE_DIR}/engine/Engine.h" "${PROJECT_SOURCE_DIR}/engine/Keyboard.cpp"
	"${PROJECT_SOURCE_DIR}/engine/Keyboard.h" "${PROJECT_SOURCE_DIR}/engine/ShaderProgram.cpp"
	"${PROJECT_SOURCE_DIR}/engine/ShaderProgram.h" "${PROJECT_SOURCE_DIR}/engine/graphics/Camera.cpp"
	"${PROJECT_SOURCE_DIR}/engine/graphics/Camera.h" "${PROJECT_SOURCE_DIR}/engine/graphics/Shader.cpp"
	"${PROJECT_SOURCE_DIR}/engine/graphics/Shader.h" 
	 
	"${PROJECT_SOURCE_DIR}/engine/graphics/Window.cpp"
	"${PROJECT_SOURCE_DIR}/engine/graphics/Window.h" 
	 
	"${PROJECT_SOURCE_DIR}/world/Chunk.cpp"
	"${PROJECT_SOURCE_DIR}/world/Chunk.h" "${PROJECT_SOURCE_DIR}/world/World.cpp"
	"${PROJECT_SOURCE_DIR}/world/World.h" "${PROJECT_SOURCE_DIR}/world/voxels/Block.cpp"
	"${PROJECT_SOURCE_DIR}/world/voxels/Block.h" "${PROJECT_SOURCE_DIR}/world/voxels/BlockDataKeyword.h"
	"${PROJECT_SOURCE_DIR}/world/voxels/BlockID.H" "${PROJECT_SOURCE_DIR}/world/voxels/Source.cpp"
	"${PROJECT_SOURCE_DIR}/Mesh_old.cpp" "${PROJECT_SOURCE_DIR}/Mesh_old.h"
	"${PROJECT_SOURCE_DIR}/Player.cpp" "${PROJECT_SOURCE_DIR}/Player.h"
	"${PROJECT_SOURCE_DIR}/Refrences.cpp" "${PROJECT_SOURCE_DIR}/Refrences.h"
	) 


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Voxels PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.

# glm
include(FetchContent)

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




