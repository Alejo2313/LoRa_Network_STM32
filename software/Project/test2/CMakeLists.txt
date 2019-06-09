
#WE WILL ROCK YOU


cmake_minimum_required(VERSION 3.0)
set(CMAKE_TOOLCHAIN_FILE STM32ToolChain.cmake)


set(PRJ_NAME LoRaNODE)

set(MCU_FAMILY STM32L0xx)
set(MCU_LINE STM32L072xx)
set(LINKER_SCRIPT STM32L072CB_FLASH.ld)
set(MCU_ARCH cortex-m0plus)


set(MODEM USE_MODEM_LORA)
set(LORA_REGION REGION_EU868)
set(LORA_BAND USE_BAND_868)

set(BSP_MCU B-L072Z-LRWAN1)
set(RADIO_DRIVER sx1276)
set(RADIO_BOARD CMWX1ZZABZ-0xx)




project(${PRJ_NAME} C ASM)

add_definitions(-DUSE_B_L072Z_LRWAN1)



add_definitions(-DCMWX1ZZABZ)
add_definitions(-DSTM32L072xx)
add_definitions(-DUSE_MODEM_LORA)
add_definitions(-DUSE_BAND_868)
add_definitions(-DREGION_EU868)
add_definitions(-DUSE_B_L072Z_LRWAN1)
add_definitions(-DUSE_HAL_LIBRARY)
add_definitions("-D__weak=__attribute__((weak))")
add_definitions("-D__packed=__attribute__((__packed__))")



file(GLOB_RECURSE CMSIS_STARTUP ../global/Drivers/CMSIS/Device/ST/STM32L0xx/Source/Templates/gcc/startup_stm32l072xx.s)

file(GLOB_RECURSE HAL_SOURCES           ../global/Drivers/STM32L0xx_HAL_Driver/Src/*.c 
                                        ../global/Drivers/CMSIS/Device/ST/STM32L0xx/Source/Templates/*.c)
file(GLOB_RECURSE BSP_SORCES            ../global/Drivers/BSP/${BSP_MCU}/*.c)
file(GLOB_RECURSE RADIO_SOURCES         ../global/Drivers/BSP/Components/${RADIO_DRIVER}/*.c)
file(GLOB_RECURSE RADIO_BSP_SOURCE      ../global/Drivers/BSP/${RADIO_BOARD}/*.c)
file(GLOB_RECURSE BOARD_CORE            ../global/Core/BOARDS/${BSP_MCU}/src/*.c)
file(GLOB_RECURSE CORE_SOURCES          ./Src/*.c
                                        ../global/Core/utils/src/*.c
                                        ../global/Core/hw/src/*.c
                                        ../global/Drivers/AD7194/src/extADC.c
                                        ../global/Drivers/AD7194/platform/STM32/src/custom_spi.c
                                        ../global/Drivers/MCP9808/src/*.c)




file(GLOB_RECURSE FREERTOS_SRC  ../global/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS/*.c
                                ../global/Middlewares/Third_Party/FreeRTOS/Source/*.c
                                ../global/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM0/*.c
                                )


#Includes 
include_directories(    ../global/Core/BOARDS/${BSP_MCU}/inc
                        ./Inc
                        ../global/Core/hw/inc
                        ../global/Core/utils/inc)

include_directories(    ../global/Drivers/${MCU_FAMILY}_HAL_Driver/Inc
                        ../global/Drivers/${MCU_FAMILY}_HAL_Driver/Inc/Legacy
                        ../global/Drivers/CMSIS/Device/ST/${MCU_FAMILY}/Include
                        ../global/Drivers/BSP/${BSP_MCU}
                        ../global/Drivers/BSP/Components/${RADIO_DRIVER}
                        ../global/Drivers/BSP/${RADIO_BOARD}
                        ../global/Drivers/CMSIS/Include 
                        ../global/Drivers/AD7194/Inc
                        ../global/Drivers/AD7194/platform/STM32/inc
                        ../global/Drivers/MCP9808/inc)


include_directories(    ../global/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS
                        ../global/Middlewares/Third_Party/FreeRTOS/Source/include
                        ../global/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM0 )



set(SOURCE_FILES   ${HAL_SOURCES} ${BSP_SORCES} ${BOARD_CORE} ${RADIO_BSP_SOURCE} ${RADIO_SOURCES} ${FREERTOS_SRC} ${LORA_CORE} ${CORE_SOURCES} ${CMSIS_STARTUP} ${MCU_LINKER_SCRIPT})

add_executable(${PRJ_NAME}.elf ${SOURCE_FILES})

set(CMAKE_EXE_LINKER_FLAGS "-Wl,-gc-sections -T ${PROJECT_SOURCE_DIR}/${LINKER_SCRIPT} -Wl,-Map=${PROJECT_SOURCE_DIR}/build/${PROJECT_NAME}.map")


set(HEX_FILE ${PROJECT_SOURCE_DIR}/build/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_SOURCE_DIR}/build/${PROJECT_NAME}.bin)

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
COMMENT "Building ${HEX_FILE} \nBuilding ${BIN_FILE}")

