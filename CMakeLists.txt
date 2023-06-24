cmake_minimum_required(VERSION 3.10)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

include(cmake/options.cmake)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_BUILD_TYPE)

if(CMAKE_BUILD_TYPE MATCHES Debug)
  message("Building in Debug mode")
elseif(CMAKE_BUILD_TYPE MATCHES Release)
  message("Building in Release mode")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

# mcu specific configs
set(MCU_FAMILY STM32F103x6)

# board specific configs
set(HSE_VALUE 8000000U)

include(cmake/toolchain.cmake)
include(cmake/flags.cmake)

set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/thirdparty/drivers/cmsis_device/Source/Templates/gcc/linker/STM32F103X6_FLASH.ld)

project(stm32f1xx-template VERSION 1.0.0 LANGUAGES C CXX ASM)

message(STATUS "MCU Family: ${MCU_FAMILY}")
message(STATUS "Compilers:")
message(STATUS "  C Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "  C++ Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "Linker script: ${LINKER_SCRIPT}")
message(STATUS "Use Full LL Driver: ${USE_FULL_LL_DRIVER}")
message(STATUS "Use HAL Driver: ${USE_HAL_DRIVER}")

add_subdirectory(core)
add_subdirectory(thirdparty)

add_subdirectory(app)
