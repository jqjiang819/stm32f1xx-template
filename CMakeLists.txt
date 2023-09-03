cmake_minimum_required(VERSION 3.12)
set(PROJECT_NAME "stm32f1xx-template")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_BUILD_TYPE)

if(CMAKE_BUILD_TYPE MATCHES Debug)
  message("Building in Debug mode")
elseif(CMAKE_BUILD_TYPE MATCHES Release)
  message("Building in Release mode")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

include(cmake/options.cmake)
include(cmake/board.cmake)
include(cmake/toolchain.cmake)
include(cmake/flags.cmake)

project(${PROJECT_NAME} VERSION 1.0.0 LANGUAGES C CXX ASM)

add_subdirectory(thirdparty)

file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.s)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
target_link_libraries(${PROJECT_NAME} PUBLIC stm32core)
target_link_libraries(${PROJECT_NAME} PRIVATE "-Wl,--print-memory-usage,-Map=${PROJECT_NAME}.map,--cref")
target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}> ${PROJECT_NAME}.hex
  COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}> ${PROJECT_NAME}.bin
  COMMAND ${CMAKE_OBJDUMP} -S $<TARGET_FILE:${PROJECT_NAME}> > ${PROJECT_NAME}.lss)

add_custom_target(flash 
  COMMAND st-flash --format ihex write ${PROJECT_NAME}.hex
  DEPENDS ${PROJECT_NAME})
