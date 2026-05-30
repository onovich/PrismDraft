cmake_minimum_required(VERSION 3.20)

project(PrismDraft
  VERSION 0.1.0
  DESCRIPTION "A constrained C11 vector-graphic 3D modeling tool"
  LANGUAGES C
)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

enable_testing()

set(PRISMDRAFT_RAYLIB_SOURCE_DIR "" CACHE PATH "Path to a local raylib source checkout.")
if(NOT PRISMDRAFT_RAYLIB_SOURCE_DIR)
  message(FATAL_ERROR "PRISMDRAFT_RAYLIB_SOURCE_DIR is required. Use tools/ConfigureBuild.ps1 to auto-detect it.")
endif()

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(PLATFORM Desktop CACHE STRING "" FORCE)
set(OPENGL_VERSION 3.3 CACHE STRING "" FORCE)
add_subdirectory(${PRISMDRAFT_RAYLIB_SOURCE_DIR} ${CMAKE_BINARY_DIR}/raylib)

add_library(prismdraft_core STATIC
  src/core/pd_core_cube_fixture.c
  src/core/pd_core_mesh_storage_controller.c
  src/core/pd_core_mesh_validator.c
  src/core/pd_core_result_entity.c
)

target_include_directories(prismdraft_core
  PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

if(MSVC)
  target_compile_options(prismdraft_core PRIVATE /W4 /WX)
else()
  target_compile_options(prismdraft_core PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

add_library(prismdraft_app STATIC
  src/app/pd_app_lifecycle_controller.c
)

target_include_directories(prismdraft_app
  PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(prismdraft_app
  PUBLIC
    prismdraft_core
)

if(MSVC)
  target_compile_options(prismdraft_app PRIVATE /W4 /WX)
else()
  target_compile_options(prismdraft_app PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

add_library(prismdraft_engine STATIC
  src/engine/pd_engine_camera_controller.c
  src/engine/pd_engine_window_config.c
)

target_include_directories(prismdraft_engine
  PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(prismdraft_engine
  PUBLIC
    raylib
)

if(MSVC)
  target_compile_options(prismdraft_engine PRIVATE /W4 /WX)
else()
  target_compile_options(prismdraft_engine PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

add_executable(pd_core_result_test
  tests/core/pd_core_result_test.c
)

target_link_libraries(pd_core_result_test
  PRIVATE
    prismdraft_core
)

if(MSVC)
  target_compile_options(pd_core_result_test PRIVATE /W4 /WX)
else()
  target_compile_options(pd_core_result_test PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

add_test(
  NAME pd_core_result_test
  COMMAND pd_core_result_test
)

add_executable(pd_core_mesh_entity_test
  tests/core/pd_core_mesh_entity_test.c
)

target_link_libraries(pd_core_mesh_entity_test
  PRIVATE
    prismdraft_core
)

if(MSVC)
  target_compile_options(pd_core_mesh_entity_test PRIVATE /W4 /WX)
else()
  target_compile_options(pd_core_mesh_entity_test PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

add_test(
  NAME pd_core_mesh_entity_test
  COMMAND pd_core_mesh_entity_test
)

add_executable(pd_core_mesh_storage_config_test
  tests/core/pd_core_mesh_storage_config_test.c
)

target_link_libraries(pd_core_mesh_storage_config_test
  PRIVATE
    prismdraft_core
)

if(MSVC)
  target_compile_options(pd_core_mesh_storage_config_test PRIVATE /W4 /WX)
else()
  target_compile_options(pd_core_mesh_storage_config_test PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

add_test(
  NAME pd_core_mesh_storage_config_test
  COMMAND pd_core_mesh_storage_config_test
)

add_executable(pd_core_mesh_storage_controller_test
  tests/core/pd_core_mesh_storage_controller_test.c
)

target_link_libraries(pd_core_mesh_storage_controller_test
  PRIVATE
    prismdraft_core
)

if(MSVC)
  target_compile_options(pd_core_mesh_storage_controller_test PRIVATE /W4 /WX)
else()
  target_compile_options(pd_core_mesh_storage_controller_test PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

add_test(
  NAME pd_core_mesh_storage_controller_test
  COMMAND pd_core_mesh_storage_controller_test
)

add_executable(pd_core_mesh_validator_test
  tests/core/pd_core_mesh_validator_test.c
)

target_link_libraries(pd_core_mesh_validator_test
  PRIVATE
    prismdraft_core
)

if(MSVC)
  target_compile_options(pd_core_mesh_validator_test PRIVATE /W4 /WX)
else()
  target_compile_options(pd_core_mesh_validator_test PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

add_test(
  NAME pd_core_mesh_validator_test
  COMMAND pd_core_mesh_validator_test
)

add_executable(pd_core_cube_fixture_test
  tests/core/pd_core_cube_fixture_test.c
)

target_link_libraries(pd_core_cube_fixture_test
  PRIVATE
    prismdraft_core
)

if(MSVC)
  target_compile_options(pd_core_cube_fixture_test PRIVATE /W4 /WX)
else()
  target_compile_options(pd_core_cube_fixture_test PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

add_test(
  NAME pd_core_cube_fixture_test
  COMMAND pd_core_cube_fixture_test
)

add_executable(pd_app_lifecycle_controller_test
  tests/app/pd_app_lifecycle_controller_test.c
)

target_link_libraries(pd_app_lifecycle_controller_test
  PRIVATE
    prismdraft_app
)

if(MSVC)
  target_compile_options(pd_app_lifecycle_controller_test PRIVATE /W4 /WX)
else()
  target_compile_options(pd_app_lifecycle_controller_test PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

add_test(
  NAME pd_app_lifecycle_controller_test
  COMMAND pd_app_lifecycle_controller_test
)

add_executable(pd_engine_window_config_test
  tests/engine/pd_engine_window_config_test.c
)

target_link_libraries(pd_engine_window_config_test
  PRIVATE
    prismdraft_engine
)

if(MSVC)
  target_compile_options(pd_engine_window_config_test PRIVATE /W4 /WX)
else()
  target_compile_options(pd_engine_window_config_test PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

add_test(
  NAME pd_engine_window_config_test
  COMMAND pd_engine_window_config_test
)

add_executable(pd_engine_camera_controller_test
  tests/engine/pd_engine_camera_controller_test.c
)

target_link_libraries(pd_engine_camera_controller_test
  PRIVATE
    prismdraft_engine
)

if(MSVC)
  target_compile_options(pd_engine_camera_controller_test PRIVATE /W4 /WX)
else()
  target_compile_options(pd_engine_camera_controller_test PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

add_test(
  NAME pd_engine_camera_controller_test
  COMMAND pd_engine_camera_controller_test
)

add_test(
  NAME prismdraft_architecture_check
  COMMAND powershell -NoProfile -ExecutionPolicy Bypass -File ${CMAKE_SOURCE_DIR}/tools/CheckArchitecture.ps1
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

add_custom_target(prismdraft_architecture_check
  COMMAND powershell -NoProfile -ExecutionPolicy Bypass -File ${CMAKE_SOURCE_DIR}/tools/CheckArchitecture.ps1
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMENT "Checking PrismDraft architecture guardrails"
)
