

include(CMakeForceCompiler)

set(MCU_ARCH -mcpu=cortex-m0plus)
#set(MCU_FLOAT_ABI -mfloat-abi=hard)
#set(MFPU -mfpu=fpv4-sp-d16)

set(CMAKE_C_COMPILER "arm-none-eabi-gcc")
set(CMAKE_CXX_COMPILER "arm-none-eabi-g++")
 
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(COMMON_FLAGS "--specs=nosys.specs ${MCU_ARCH} -std=gnu11 --no-warn -mthumb -mno-thumb-interwork ${MFPU} ${MCU_FLOAT_ABI} -ffunction-sections -fdata-sections -Wall -g -fno-common -fmessage-length=0")
set(CMAKE_C_FLAGS "${COMMON_FLAGS} -std=gnu11 ")
set(CMAKE_CXX_FLAGS "${COMMON_FLAGS} -std=c++11 ")

set(HEX_FILE ${PROJECT_SOURCE_DIR}/build/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_SOURCE_DIR}/build/${PROJECT_NAME}.bin)


set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# Avoid known bug in linux giving: 
#    arm-none-eabi-gcc: error: unrecognized command line option '-rdynamic'
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")
