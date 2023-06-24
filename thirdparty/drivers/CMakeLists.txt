# add cmsis
add_library(cmsis INTERFACE)
target_include_directories(cmsis INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/cmsis_core/Include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/cmsis_device/Include>)

# add hal driver
set(HAL_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/stm32f1xx_hal/Src)
file(GLOB_RECURSE HAL_SOURCES CONFIGURE_DEPENDS ${HAL_SOURCE_DIR}/stm32*_hal_*.c ${HAL_SOURCE_DIR}/stm32*_hal.c)
file(GLOB_RECURSE LL_SOURCES CONFIGURE_DEPENDS ${HAL_SOURCE_DIR}/stm32*_ll_*.c ${HAL_SOURCE_DIR}/stm32*_ll.c)

set(HAL_LIB_SOURCES ${LL_SOURCES})

if(USE_HAL_DRIVER)
  list(APPEND HAL_LIB_SOURCES ${HAL_SOURCES})
endif()

list(FILTER HAL_LIB_SOURCES EXCLUDE REGEX ".*_template.c$")

add_library(stm32hal INTERFACE)
target_sources(stm32hal INTERFACE ${HAL_LIB_SOURCES})
target_link_libraries(stm32hal INTERFACE cmsis)
target_include_directories(stm32hal INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/stm32f1xx_hal/Inc>)
