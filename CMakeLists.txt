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
