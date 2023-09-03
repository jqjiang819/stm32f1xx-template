# core flags
set(CORE_FLAGS "-mthumb -mcpu=cortex-m3")
# common flags
set(COMMON_FLAGS "-Wall -fdata-sections -ffunction-sections")
# optimization flags
if(CMAKE_BUILD_TYPE MATCHES Debug)
  set(OPT_FLAGS "-g -gdwarf-2 -Og")
elseif(CMAKE_BUILD_TYPE MATCHES Release)
  set(OPT_FLAGS "-Os")
endif()
# definitions
if(USE_HAL_DRIVER)
  string(APPEND DEF_FLAGS " -DUSE_HAL_DRIVER")
endif()
if(USE_FULL_LL_DRIVER)
  string(APPEND DEF_FLAGS " -DUSE_FULL_LL_DRIVER")
endif()
string(APPEND DEF_FLAGS " -D${MCU_VARIANT}")

set(CMAKE_C_FLAGS "${CORE_FLAGS} ${OPT_FLAGS} ${COMMON_FLAGS} ${DEF_FLAGS}" CACHE INTERNAL "C Compiler Flags")
set(CMAKE_C_FLAGS_DEBUG "" CACHE INTERNAL "C Compiler Debug Flags")
set(CMAKE_C_FLAGS_RELEASE "" CACHE INTERNAL "C Compiler Release Flags")

set(CMAKE_CXX_FLAGS "${CORE_FLAGS} ${OPT_FLAGS} ${COMMON_FLAGS} ${DEF_FLAGS}" CACHE INTERNAL "CXX Compiler Flags")
set(CMAKE_CXX_FLAGS_DEBUG "" CACHE INTERNAL "CXX Compiler Debug Flags")
set(CMAKE_CXX_FLAGS_RELEASE "" CACHE INTERNAL "CXX Compiler Release Flags")

set(CMAKE_ASM_FLAGS "-x assembler-with-cpp ${CORE_FLAGS} ${OPT_FLAGS} ${COMMON_FLAGS}" CACHE INTERNAL "ASM Compiler Flags")
set(CMAKE_ASM_FLAGS_DEBUG "" CACHE INTERNAL "ASM Compiler Debug Flags")
set(CMAKE_ASM_FLAGS_RELEASE "" CACHE INTERNAL "ASM Compiler Release Flags")

set(CMAKE_EXE_LINKER_FLAGS "${CORE_FLAGS} -specs=nano.specs -T${LINKER_SCRIPT} -lc -lm -lnosys -Wl,--gc-sections" CACHE INTERNAL "Linker Flags")
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "" CACHE INTERNAL "Shared Linker Flags")

message(STATUS "Flags:")
message(STATUS " - C Compiler Flags: ${CMAKE_C_FLAGS}")
message(STATUS " - CXX Compiler Flags: ${CMAKE_CXX_FLAGS}")
message(STATUS " - ASM Compiler Flags: ${CMAKE_ASM_FLAGS}")
message(STATUS " - Linker Flags: ${CMAKE_EXE_LINKER_FLAGS}")
