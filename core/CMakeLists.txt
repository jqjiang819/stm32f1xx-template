set(GENERATED_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)

set(TPL_HAL_CONF_HEADER ${CMAKE_SOURCE_DIR}/thirdparty/drivers/stm32f1xx_hal/Inc/${MCU_TYPE_LOWER}_hal_conf_template.h)
set(TPL_SYSTEM_SOURCE ${CMAKE_SOURCE_DIR}/thirdparty/drivers/cmsis_device/Source/Templates/system_${MCU_TYPE_LOWER}.c)
set(TPL_STARTUP_SCRIPT ${CMAKE_SOURCE_DIR}/thirdparty/drivers/cmsis_device/Source/Templates/gcc/startup_${MCU_FAMILY_LOWER}.s)

set(CUST_HAL_CONF_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/include/${MCU_TYPE_LOWER}_hal_conf.h)
set(CUST_SYSTEM_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/src/system_${MCU_TYPE_LOWER}.c)
set(CUST_STARTUP_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/src/startup_${MCU_FAMILY_LOWER}.s)

if(NOT EXISTS ${CUST_HAL_CONF_HEADER})
  message(STATUS "Using template HAL config header file")
  file(COPY ${TPL_HAL_CONF_HEADER} DESTINATION ${GENERATED_DIR}/include)
  file(RENAME ${GENERATED_DIR}/include/${MCU_TYPE_LOWER}_hal_conf_template.h ${GENERATED_DIR}/include/${MCU_TYPE_LOWER}_hal_conf.h)
else()
  message(STATUS "Using custom HAL config header file")
  file(REMOVE ${GENERATED_DIR}/include/${MCU_TYPE_LOWER}_hal_conf.h)
endif()

if(NOT EXISTS ${CUST_SYSTEM_SOURCE})
  message(STATUS "Using template system source file")
  file(COPY ${TPL_SYSTEM_SOURCE} DESTINATION ${GENERATED_DIR}/src)
else()
  message(STATUS "Using custom system source file")
  file(REMOVE ${GENERATED_DIR}/src/system_${MCU_TYPE_LOWER}.c)
endif()

if(NOT EXISTS ${CUST_STARTUP_SCRIPT})
  message(STATUS "Using template startup script")
  file(COPY ${TPL_STARTUP_SCRIPT} DESTINATION ${GENERATED_DIR}/src)
else()
  message(STATUS "Using custom startup script")
  file(REMOVE ${GENERATED_DIR}/src/startup_${MCU_FAMILY_LOWER}.s)
endif()

file(GLOB_RECURSE CORE_CUST_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c ${CMAKE_CURRENT_SOURCE_DIR}/src/*.s)
file(GLOB_RECURSE CORE_GEN_SOURCES CONFIGURE_DEPENDS ${GENERATED_DIR}/src/*.c ${GENERATED_DIR}/src/*.s)

add_library(core INTERFACE)
target_sources(core INTERFACE ${CORE_CUST_SOURCES} ${CORE_GEN_SOURCES})
target_include_directories(core INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<BUILD_INTERFACE:${GENERATED_DIR}/include>)
