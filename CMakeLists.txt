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
