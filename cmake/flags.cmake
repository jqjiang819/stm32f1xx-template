if(CMAKE_BUILD_TYPE MATCHES Debug)
  set(DEBUG_FLAGS "-g3 -gdwarf-2 -O0")
elseif(CMAKE_BUILD_TYPE MATCHES Release)
  set(DEBUG_FLAGS "-Os")
endif()

string(REGEX REPLACE "^(STM32[A-Z]1).*$" "\\1xx" MCU_TYPE ${MCU_FAMILY})
string(TOLOWER ${MCU_TYPE} MCU_TYPE_LOWER)
string(TOLOWER ${MCU_FAMILY} MCU_FAMILY_LOWER)

if(MCU_FAMILY MATCHES "^STM32F1.*$")
  set(CORE_FLAGS "-mthumb -mcpu=cortex-m3")
elseif(MCU_FAMILY MATCHES "^STM32F4.*$")
  set(CORE_FLAGS "-mthumb -mcpu=cortex-m4")
elseif(MCU_FAMILY MATCHES "^STM32(F|H)7.*$")
  set(CORE_FLAGS "-mthumb -mcpu=cortex-m7")
else()
  message(WARNING "Unsupported MCU Family: ${MCU_FAMILY}")
endif()

string(APPEND EXTRA_FLAGS "-D${MCU_FAMILY}")

if(USE_HAL_DRIVER)
  string(APPEND EXTRA_FLAGS " -DUSE_HAL_DRIVER")
endif()

if(USE_FULL_LL_DRIVER)
  string(APPEND EXTRA_FLAGS " -DUSE_FULL_LL_DRIVER")
endif()

message(STATUS "CORE_FLAGS: ${CORE_FLAGS}")
message(STATUS "DEBUG_FLAGS: ${DEBUG_FLAGS}")
message(STATUS "EXTRA_FLAGS: ${EXTRA_FLAGS}")

set(CMAKE_C_FLAGS "${CORE_FLAGS} -std=gnu99 -Wall -fdata-sections -ffunction-sections ${DEBUG_FLAGS} ${EXTRA_FLAGS}" CACHE INTERNAL "C compiler flags")
set(CMAKE_C_FLAGS_DEBUG "" CACHE INTERNAL "C compiler flags: Debug")
set(CMAKE_C_FLAGS_RELEASE "" CACHE INTERNAL "C compiler flags: Release")

set(CMAKE_CXX_FLAGS "${CORE_FLAGS} -fno-rtti -fno-exceptions -fno-builtin -Wall -fdata-sections -ffunction-sections ${DEBUG_FLAGS} ${EXTRA_FLAGS}" CACHE INTERNAL "Cxx compiler flags")
set(CMAKE_CXX_FLAGS_DEBUG "" CACHE INTERNAL "Cxx compiler flags: Debug")
set(CMAKE_CXX_FLAGS_RELEASE "" CACHE INTERNAL "Cxx compiler flags: Release")

set(CMAKE_ASM_FLAGS "${CORE_FLAGS} -x assembler-with-cpp ${DEBUG_FLAGS} ${EXTRA_FLAGS}" CACHE INTERNAL "ASM compiler flags")
set(CMAKE_ASM_FLAGS_DEBUG "" CACHE INTERNAL "ASM compiler flags: Debug")
set(CMAKE_ASM_FLAGS_RELEASE "" CACHE INTERNAL "ASM compiler flags: Release")

set(CMAKE_EXE_LINKER_FLAGS "${CORE_FLAGS} -Wl,--gc-sections" CACHE INTERNAL "Exe linker flags")
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "" CACHE INTERNAL "Shared linker flags")
