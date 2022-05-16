#cmake_minimum_required(VERSION 3.10)

#project(FreeRTOS_LwIP__HTTP_SLIP LANGUAGES C ASM)

#set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
#SET(CMAKE_C_FLAGS "-mthumb -fno-builtin -mcpu=cortex-m3 -Wall -std=gnu11 -ffunction-sections -fdata-sections -fomit-frame-pointer -mabi=aapcs -fno-unroll-loops -ffast-math -ftree-vectorize" CACHE INTERNAL "c compiler flags")
#SET(CMAKE_CXX_FLAGS "-mthumb -fno-builtin -mcpu=cortex-m3 -Wall -std=gnu++11 -ffunction-sections -fdata-sections -fomit-frame-pointer -mabi=aapcs -fno-unroll-loops -ffast-math -ftree-vectorize" CACHE INTERNAL "cxx compiler flags")
#SET(CMAKE_ASM_FLAGS "-mthumb -mcpu=cortex-m3 -x assembler-with-cpp" CACHE INTERNAL "asm compiler flags")

#SET(CMAKE_EXE_LINKER_FLAGS "-Wl,--gc-sections -mthumb -mcpu=cortex-m3 -mabi=aapcs" CACHE INTERNAL "executable linker flags")
#SET(CMAKE_MODULE_LINKER_FLAGS "-mthumb -mcpu=cortex-m3 -mabi=aapcs" CACHE INTERNAL "module linker flags")
#SET(CMAKE_SHARED_LINKER_FLAGS "-mthumb -mcpu=cortex-m3 -mabi=aapcs" CACHE INTERNAL "shared linker flags")

#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T ${CMAKE_SOURCE_DIR}/link.ld")

## Resonator used in this project. Stm32F4-DISCO uses 8MHz crystal. I left this definition here
## in the CMakeLists.txt rather than the toolchain file, because it's project dependent, not
## "platform" dependent, where by platform I mean STM32F4.
#ADD_DEFINITIONS (-DSTM32F407xx)
#ADD_DEFINITIONS (-DPRIO_LWIP=3)
#ADD_DEFINITIONS (-DLWIP_DEBUG)


## Startup code is written by ST in assembly, so without this statement there are errors.
##ENABLE_LANGUAGE (ASM-ATT)

##set(CMAKE_C_COMPILER /home/macias/opt/xPacks/arm-none-eabi-gcc/xpack-arm-none-eabi-gcc-9.2.1-1.1/bin/arm-none-eabi-gcc)
##set(CMAKE_CXX_COMPILER /home/macias/opt/xPacks/arm-none-eabi-gcc/xpack-arm-none-eabi-gcc-9.2.1-1.1/bin/arm-none-eabi-g++)
#file(GLOB include_glob
#    "Drivers/"
#    "src/"
#    "src/console_mod/"
##            //OS
#    "FreeRTOS/include/"
#    "FreeRTOS/include/private/"
#//        "FreeRTOS/FreeRTOS/"
#    "FreeRTOS/FreeRTOS/portable/GCC/ARM_CM4F/"
#    "FreeRTOS/Trace/Streaming/"
##        //lwiP
#    "lwip/"
#    "lwip/include/"
#    "lwip/httpd/ssi/"
#    "lwip/snmp/"

#    "src/snmp/"
##        //apps programm thast include from telnet / console
#    "src/apps/ebi_rw-master/"
#    "src/apps/ebi_rw-master/"
##//            //telNet
##//        "src/TelNet*.h"
#)
#set(include
#${include_glob}
#)

#INCLUDE_DIRECTORIES(${include})

## All the sources goes here. Adding headers isn't obligatory, but since QtCreator treats CMakeLists.txt as
## its "project configuration" it simply makes header files appear in the source tree pane.

#file(GLOB src_glob
#    "Drivers/*"
#    "src/*"
#    "src/console_mod/*"
##            //OS
#    "FreeRTOS/include/*"
#    "FreeRTOS/include/private/*"
#    "FreeRTOS/FreeRTOS/*"
#    "FreeRTOS/FreeRTOS/portable/GCC/ARM_CM4F/*"
#    "FreeRTOS/FreeRTOS/portable/MemMang/heap_2.c"
#    "FreeRTOS/Trace/Streaming/*"
##        //lwiP  =  lwip     lwip_povorotnoe    lwip_old
#    "lwip/api/*"
#    "lwip/arch/*"
#    "lwip/core/**"
#    "lwip/httpd/*"
#    "lwip/lwiperf/*"
#    "lwip/snmp/*"
#    "lwip/httpd/ssi/*"
#    "lwip/httpd/post/*"
#    "lwip/netif/*"
#    "lwip/include/*"

#    "src/snmp/*"
##        //apps programm thast include from telnet / console
#    "src/apps/ebi_rw-master/*.c"
#    "src/apps/ebi_rw-master/*.h"
##//            //telNet
##//        "src/TelNet/*"
#)
#set(src
#${src_glob}
#)

#add_executable(${CMAKE_PROJECT_NAME}
#    ${src}
#)

## Workaround : splitting C and C++ code helps QtCreator parse header files correctly. Without it, QtCreator
## sometimes treats C++ as C and vice versa. EDIT : this comment was written when in the ADD_EXECUTABLE C++
## files were present.
##add_library ("stm" STATIC
## "${CUBE_ROOT}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c"
## "${CUBE_ROOT}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c"
## "${CUBE_ROOT}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c"
## "${CUBE_ROOT}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c"
## "${CUBE_ROOT}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c"
## "${CUBE_ROOT}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_gpio.c"
## "${CUBE_ROOT}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_rcc.c"
## "${CUBE_ROOT}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_utils.c"
## "${CUBE_ROOT}/Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_cortex.h"
## "${CUBE_ROOT}/Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_gpio_ex.h"
## "${CUBE_ROOT}/Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_gpio.h"
## "${CUBE_ROOT}/Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal.h"
## "${CUBE_ROOT}/Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h"
## "${CUBE_ROOT}/Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc.h"
## "${CUBE_ROOT}/Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_cortex.h"
## "${CUBE_ROOT}/Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_gpio.h"
## "${CUBE_ROOT}/Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_rcc.h"
## "${CUBE_ROOT}/Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_system.h"
## "${STARTUP_CODE}"
##)

## This links both pieces together.
##TARGET_LINK_LIBRARIES (${CMAKE_PROJECT_NAME}.elf -Wl,--whole-archive stm -Wl,--no-whole-archive)

##FIND_PROGRAM (OPENOCD openocd)
##ADD_CUSTOM_TARGET("upload" DEPENDS ${CMAKE_PROJECT_NAME}.elf COMMAND ${OPENOCD} -f /usr/share/openocd/scripts/interface/stlink-v2.cfg -f /usr/share/openocd/scripts/target/stm32f4x.cfg -c 'program ${CMAKE_PROJECT_NAME}.elf verify reset exit')
cmake_minimum_required(VERSION 3.5)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

#no use flag:
#"-msoft-float",
#"-flto",      //это какой-то оптимизатор из за него не собираеться когда все вместе
#SET(CMAKE_C_FLAGS
#    "-mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -fno-strict-aliasing -fdata-sections -ffunction-sections -fshort-enums -finput-charset=UTF-8 -fexec-charset=cp1251 -std=gnu11 -Wall -Wpedantic -Wno-main -Wno-unused-function -Wno-old-style-declaration -specs=nano.specs"
#    CACHE INTERNAL "c compiler flags")
SET(CMAKE_C_FLAGS
    "-mthumb -fno-builtin -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -fno-strict-aliasing -Wall -std=gnu11 -ffunction-sections -fdata-sections -fomit-frame-pointer -mabi=aapcs -fno-unroll-loops -ffast-math -ftree-vectorize"
    CACHE INTERNAL "c compiler flags")
#SET(CMAKE_CXX_FLAGS "-mthumb -fno-builtin -mcpu=cortex-m3 -Wall -std=gnu++11 -ffunction-sections -fdata-sections -fomit-frame-pointer -mabi=aapcs -fno-unroll-loops -ffast-math -ftree-vectorize" CACHE INTERNAL "cxx compiler flags")
SET(CMAKE_CXX_FLAGS ${CMAKE_C_FLAGS})
#SET(CMAKE_CXX_FLAGS "-mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -fno-strict-aliasing -fdata-sections -ffunction-sections -fshort-enums -finput-charset=UTF-8 -fexec-charset=cp1251 -std=gnu11 -Wall -Wpedantic -Wno-main -Wno-unused-function -Wno-old-style-declaration -specs=nano.specs"
#    CACHE INTERNAL "c compiler flags")
#SET(CMAKE_ASM_FLAGS "-mthumb -mcpu=cortex-m3 -x assembler-with-cpp" CACHE INTERNAL "asm compiler flags")
SET(CMAKE_ASM_FLAGS ${CMAKE_C_FLAGS})
#SET(CMAKE_ASM_FLAGS "-mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -fno-strict-aliasing -fdata-sections -ffunction-sections -fshort-enums -finput-charset=UTF-8 -fexec-charset=cp1251 -std=gnu11 -Wall -Wpedantic -Wno-main -Wno-unused-function -Wno-old-style-declaration -specs=nano.specs"
#    CACHE INTERNAL "c compiler flags")

#SET(CMAKE_EXE_LINKER_FLAGS "-Wl,--gc-sections -mthumb -mcpu=cortex-m3 -mabi=aapcs" CACHE INTERNAL "executable linker flags")
SET(CMAKE_EXE_LINKER_FLAGS "-Wl,--gc-sections -Wl,-start-group -lnosys -mthumb -nostdlib" CACHE INTERNAL "executable linker flags")
#SET(CMAKE_MODULE_LINKER_FLAGS "-mthumb -mcpu=cortex-m3 -mabi=aapcs" CACHE INTERNAL "module linker flags")
SET(CMAKE_MODULE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} CACHE INTERNAL "module linker flags")
#SET(CMAKE_SHARED_LINKER_FLAGS "-mthumb -mcpu=cortex-m3 -mabi=aapcs" CACHE INTERNAL "shared linker flags")
SET(CMAKE_SHARED_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} CACHE INTERNAL "shared linker flags")

project(FreeRTOS_LwIP__HTTP_SLIP LANGUAGES C ASM)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T ${CMAKE_SOURCE_DIR}/STM32F407VGTx_FLASH.ld")

add_definitions(-DSTM32F407xx)
add_definitions(-DPRIO_LWIP=3)
add_definitions(-DLWIP_DEBUG)

include_directories(
        "Drivers/"
        "src/"
        "src/console_mod/"
    #            //OS
        "FreeRTOS/include/"
        "FreeRTOS/include/private/"
    //        "FreeRTOS/FreeRTOS/"
        "FreeRTOS/FreeRTOS/portable/GCC/ARM_CM4F/"
        "FreeRTOS/Trace/Streaming/"
    #        //lwiP
        "lwip/"
        "lwip/include/"
        "lwip/httpd/ssi/"
        "lwip/snmp/"

        "src/snmp/"
    #        //apps programm thast include from telnet / console
        "src/apps/ebi_rw-master/"
    #//            //telNet
    #//        "src/TelNet*.h"
    )

file(GLOB stdPeriph
        "Drivers/*"
        "src/*.c"
        "src/console_mod/*.c"
    #            //OS
        "FreeRTOS/include/*.c"
        "FreeRTOS/include/private/*.c"
        "FreeRTOS/FreeRTOS/*.c"
        "FreeRTOS/FreeRTOS/portable/GCC/ARM_CM4F/*.c"
        "FreeRTOS/FreeRTOS/portable/MemMang/heap_2.c"
        "FreeRTOS/Trace/Streaming/*.c"
    #        //lwiP  =  lwip     lwip_povorotnoe    lwip_old
        "lwip/api/*.c"
        "lwip/arch/*.c"
        "lwip/core/*.c"
        "lwip/core/ipv4/*.c"
        "lwip/core/ipv6/*.c"
        "lwip/httpd/*.c"
        "lwip/lwiperf/*.c"
        "lwip/snmp/*.c"
        "lwip/httpd/ssi/*.c"
        "lwip/httpd/post/*.c"
        "lwip/netif/*.c"
        "lwip/include/*.c"

        "src/snmp/*.c"
    #        //apps programm thast include from telnet / console
        "src/apps/ebi_rw-master/*.c"
    #//            //telNet
    #//        "src/TelNet/*"
    )

add_executable(${CMAKE_PROJECT_NAME} ${stdPeriph})
