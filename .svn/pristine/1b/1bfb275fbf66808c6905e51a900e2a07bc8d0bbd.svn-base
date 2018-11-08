include(CMakeForceCompiler)

set(MCU_LINKER_SCRIPT STM32L072CB_FLASH.ld)
set (MCU_FLOAT_ABI soft)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_C_COMPILER "arm-none-eabi-gcc")
set(CMAKE_CXX_COMPILER "arm-none-eabi-g++")

set(COMMON_FLAGS "--specs=nosys.specs -mcpu=${MCU_ARCH} -std=gnu11 --no-warn -mthumb -mthumb-interwork -mfloat-abi=${MCU_FLOAT_ABI} -ffunction-sections -fdata-sections -g -fno-common -fmessage-length=0")


set(CMAKE_CXX_FLAGS "${COMMON_FLAGS} -std=c++11 ")
set(CMAKE_C_FLAGS "${COMMON_FLAGS} -std=gnu11 ")
set(CMAKE_EXE_LINKER_FLAGS "-Wl,-gc-sections -T ${CMAKE_SOURCE_DIR}/${MCU_LINKER_SCRIPT}")


set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)