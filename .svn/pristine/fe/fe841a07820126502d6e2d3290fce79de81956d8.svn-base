
#WE WILL ROCK YOU


cmake_minimum_required(VERSION 3.0)
set(CMAKE_TOOLCHAIN_FILE STM32ToolChain.cmake)

################################################################################
#SOLO SE CAMBIA ESTO!!!! 

set(PRJ_NAME LoRaNODE)

set(MCU_FAMILY STM32L0xx)
set(MCU_LINE STM32L072xx)
set(MCU_LINKER_SCRIPT STM32L072CB_FLASH.ld)
set(MCU_ARCH cortex-m0plus)


set(MODEM USE_MODEM_LORA)
set(LORA_REGION REGION_EU868)
set(LORA_BAND USE_BAND_868)

set(BSP_MCU B-L072Z-LRWAN1)
set(RADIO_DRIVER sx1276)
set(RADIO_BOARD CMWX1ZZABZ-0xx)


#############################################################################


project(${PRJ_NAME} C ASM)



if(${BSP_MCU} STREQUAL B-L072Z-LRWAN1 )
        add_definitions(-DUSE_B_L072Z_LRWAN1)

elseif(${BSP_MUCU} STREQUAL STM32L4xx_Nucleo)
        add_definitions(-DUSE_STM32L4XX_NUCLEO)
endif()


add_definitions(-D${MCU_LINE})
add_definitions(-D${MODEM})
add_definitions(-D${LORA_BAND})
add_definitions(-D${LORA_REGION})
add_definitions(-DUSE_${BSP_MCU})
add_definitions(-DUSE_HAL_LIBRARY)
add_definitions("-D__weak=__attribute__((weak))")
add_definitions("-D__packed=__attribute__((__packed__))")


if(${MCU_LINE} STREQUAL STM32L072xx)
        file(GLOB_RECURSE CMSIS_STARTUP startup/startup_stm32l072xx.s)
elseif(${MCU_LINE} STREQUAL STM32L432xx)
        file(GLOB_RECURSE CMSIS_STARTUP startup/startup_stm32l072xx.s)
endif()


file(GLOB_RECURSE HAL_SOURCES           Drivers/${MCU_FAMILY}_HAL_Driver/Src/*.c 
                                        Drivers/CMSIS/Device/ST/${MCU_FAMILY}/Source/Templates/*.c)
file(GLOB_RECURSE BSP_SORCES            Drivers/BSP/${BSP_MCU}/*.c)
file(GLOB_RECURSE RADIO_SOURCES         Drivers/BSP/Components/${RADIO_DRIVER}/*.c)
file(GLOB_RECURSE RADIO_BSP_SOURCE      Drivers/BSP/${RADIO_BOARD}/*.c)
file(GLOB_RECURSE BOARD_CORE            Core/BOARDS/${BSP_MCU}/src/*.c)
file(GLOB_RECURSE CORE_SOURCES          Core/Src/*.c)


#Middleware
file(GLOB_RECURSE LORA_CORE     Middlewares/Third_Party/LoRaWAN/Crypto/*.c 
                                Middlewares/Third_Party/LoRaWAN/Mac/*c 
                                Middlewares/Third_Party/LoRaWAN/Mac/region/*.c
                                Middlewares/Third_Party/LoRaWAN/Phy/*.c
                                Middlewares/Third_Party/LoRaWAN/Utilities/*.c )


file(GLOB_RECURSE FREERTOS_SRC  Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS/*.c
                                Middlewares/Third_Party/FreeRTOS/Source/*.c
                                Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM0/*.c
                                )


#Includes 

include_directories(    Drivers/${MCU_FAMILY}_HAL_Driver/Inc
                        Drivers/${MCU_FAMILY}_HAL_Driver/Inc/Legacy
                        Drivers/CMSIS/Device/ST/${MCU_FAMILY}/Include
                        Drivers/BSP/${BSP_MCU}
                        Drivers/BSP/Components/${RADIO_DRIVER}
                        Drivers/BSP/${RADIO_BOARD})

include_directories(    Middlewares/Third_Party/LoRaWAN/Crypto 
                        Middlewares/Third_Party/LoRaWAN/Mac 
                        Middlewares/Third_Party/LoRaWAN/Mac/region
                        Middlewares/Third_Party/LoRaWAN/Phy
                        Middlewares/Third_Party/LoRaWAN/Utilities
                        Drivers/CMSIS/Include )

include_directories(    Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS
                        Middlewares/Third_Party/FreeRTOS/Source/include
                        Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM0 )

include_directories(   Core/BOARDS/${BSP_MCU}/inc
                       Core/Inc)



set(SOURCE_FILES   ${HAL_SOURCES} ${BSP_SORCES} ${BOARD_CORE} ${RADIO_BSP_SOURCE} ${RADIO_SOURCES} ${FREERTOS_SRC} ${LORA_CORE} ${CORE_SOURCES} ${CMSIS_STARTUP} ${MCU_LINKER_SCRIPT})

add_executable(${PRJ_NAME}.elf ${SOURCE_FILES})

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${PROJECT_SOURCE_DIR}/build/${PROJECT_NAME}.map  ")
set(HEX_FILE ${PROJECT_SOURCE_DIR}/build/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_SOURCE_DIR}/build/${PROJECT_NAME}.bin)

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
COMMENT "Building ${HEX_FILE} \nBuilding ${BIN_FILE}")