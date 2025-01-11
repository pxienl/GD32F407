cmake_minimum_required(VERSION 3.22)

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

set(CMAKE_SYSTEM_NAME               Generic)
set(CMAKE_SYSTEM_PROCESSOR          arm)

set(CMAKE_C_COMPILER_FORCED TRUE)
set(CMAKE_CXX_COMPILER_FORCED TRUE)
set(CMAKE_C_COMPILER_ID GNU)
set(CMAKE_CXX_COMPILER_ID GNU)

# Some default GCC settings
# arm-none-eabi- must be part of path environment
set(TOOLCHAIN_PREFIX                arm-none-eabi-)

set(CMAKE_C_COMPILER                ${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_ASM_COMPILER              ${CMAKE_C_COMPILER})
set(CMAKE_CXX_COMPILER              ${TOOLCHAIN_PREFIX}g++)
set(CMAKE_LINKER                    ${TOOLCHAIN_PREFIX}g++)
set(CMAKE_OBJCOPY                   ${TOOLCHAIN_PREFIX}objcopy)
set(CMAKE_SIZE                      ${TOOLCHAIN_PREFIX}size)

set(CMAKE_EXECUTABLE_SUFFIX_ASM     ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_C       ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_CXX     ".elf")

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# MCU specific flags
set(TARGET_FLAGS "-mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard ")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${TARGET_FLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic -fdata-sections -ffunction-sections")
if(CMAKE_BUILD_TYPE MATCHES Debug)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -g3")
endif()
if(CMAKE_BUILD_TYPE MATCHES Release)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Os -g0")
endif()

set(CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS} -x assembler-with-cpp -MMD -MP")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -fno-rtti -fno-exceptions -fno-threadsafe-statics")

set(CMAKE_C_LINK_FLAGS "${TARGET_FLAGS}")
set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -T \"${CMAKE_SOURCE_DIR}/gd32f4xx_flash.ld\"")
set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} --specs=nano.specs")
set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -Wl,-Map=${CMAKE_PROJECT_NAME}.map -Wl,--gc-sections")
set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -Wl,--start-group -lc -lm -Wl,--end-group")
set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -Wl,--print-memory-usage")

set(CMAKE_CXX_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -Wl,--start-group -lstdc++ -lsupc++ -Wl,--end-group")

# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME GD32F407)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

#Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
    ./Firmware/CMSIS/GD/GD32F4xx/Source/system_gd32f4xx.c
    ./Firmware/CMSIS/gcc_startup/startup_gd32f407.S

    ./Firmware/GD32F4xx_standard_peripheral/Source/gd32f4xx_rcu.c
    ./Firmware/GD32F4xx_standard_peripheral/Source/gd32f4xx_gpio.c
    ./Firmware/GD32F4xx_standard_peripheral/Source/gd32f4xx_misc.c
    ./Firmware/GD32F4xx_standard_peripheral/Source/gd32f4xx_usart.c
    ./Firmware/GD32F4xx_standard_peripheral/Source/gd32f4xx_syscfg.c
    ./Firmware/GD32F4xx_standard_peripheral/Source/gd32f4xx_exti.c
    ./Firmware/GD32F4xx_standard_peripheral/Source/gd32f4xx_timer.c
    ./Firmware/GD32F4xx_standard_peripheral/Source/gd32f4xx_dma.c
    ./Firmware/GD32F4xx_standard_peripheral/Source/gd32f4xx_pmu.c
    ./Firmware/GD32F4xx_standard_peripheral/Source/gd32f4xx_spi.c

    ./Library/USART.c
    ./Library/EXTI.c
    ./Library/Timer.c
    ./Library/GPIO.c
    ./Library/DMA.c
    ./Library/SPI.c

    ./User/gd32f4xx_it.c
    ./User/main.c
    ./User/systick.c
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
    ./Firmware/CMSIS
    ./Firmware/CMSIS/GD/GD32F4xx/Include
    ./Firmware/GD32F4xx_standard_peripheral/Include

    ./User
    ./Library
    ./Hardware
    ./Third_Party
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
    USE_STDPERIPH_DRIVER
    GD32F407
    $<$<CONFIG:Debug>:DEBUG>
)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}

    # Add user defined libraries
)



if(CMAKE_C_STANDARD LESS 11)
    message(ERROR "Generated code requires C11 or higher")
endif()