cmake_minimum_required(VERSION "3.30")

if(NOT DEFINED CMAKE_SYSTEM_NAME)
    set(CMAKE_SYSTEM_NAME Generic)
endif()
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

if(NOT DEFINED CMAKE_C_COMPILER)
    set(CMAKE_C_COMPILER arm-none-eabi-gcc CACHE STRING "C compiler" FORCE)
endif()
if(NOT DEFINED CMAKE_CXX_COMPILER)
    set(CMAKE_CXX_COMPILER arm-none-eabi-g++ CACHE STRING "C++ compiler" FORCE)
endif()
if(NOT DEFINED CMAKE_ASM_COMPILER)
    set(CMAKE_ASM_COMPILER arm-none-eabi-gcc CACHE STRING "ASM compiler" FORCE)
endif()

project(bm_self C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(
    -Os
    -mthumb
    -mcpu=cortex-m0plus
    -ffreestanding
    -nostdlib
    -g
)

add_link_options(
    -nostdlib
    -mthumb
    -mcpu=cortex-m0plus
)

find_program(ARM_OBJCOPY arm-none-eabi-objcopy REQUIRED)
find_package(Python3 REQUIRED COMPONENTS Interpreter)

add_subdirectory(src/kerl)

add_library(rt_obj OBJECT src/rt.c)

set(BOOT2_BIN ${CMAKE_BINARY_DIR}/rt.bin)
add_custom_command(
    OUTPUT ${BOOT2_BIN}
    COMMAND ${ARM_OBJCOPY} -O binary -j .boot2 $<TARGET_OBJECTS:rt_obj> ${BOOT2_BIN}
    DEPENDS rt_obj
    COMMENT "Extracting boot2 binary"
)

set(CRC_C ${CMAKE_BINARY_DIR}/crc.c)
set_source_files_properties(${CRC_C} PROPERTIES GENERATED TRUE)
add_custom_command(
    OUTPUT ${CRC_C}
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/calc_crc.py ${BOOT2_BIN} ${CRC_C}
    DEPENDS ${BOOT2_BIN} ${CMAKE_SOURCE_DIR}/calc_crc.py
    COMMENT "Generating CRC section"
)

add_library(crc_obj OBJECT ${CRC_C})

set(app_sources "")
if(EXISTS "${CMAKE_SOURCE_DIR}/src/main.c")
    list(APPEND app_sources ${CMAKE_SOURCE_DIR}/src/main.c)
endif()

add_executable(image.elf
    ${app_sources}
    $<TARGET_OBJECTS:rt_obj>
    $<TARGET_OBJECTS:crc_obj>
)

target_link_libraries(image.elf PRIVATE kerl)

target_link_options(image.elf PRIVATE
    -T ${CMAKE_SOURCE_DIR}/rp2040.ld
    -Wl,-Map=${CMAKE_BINARY_DIR}/image.map
)

add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/image.bin
    COMMAND ${ARM_OBJCOPY} -O binary $<TARGET_FILE:image.elf> ${CMAKE_BINARY_DIR}/image.bin
    DEPENDS image.elf
    COMMENT "Creating image.bin"
)

add_custom_target(image ALL DEPENDS ${CMAKE_BINARY_DIR}/image.bin)

add_custom_target(gdb
    COMMAND gdb-multiarch -x ${CMAKE_SOURCE_DIR}/runscript.gdb ${CMAKE_BINARY_DIR}/image.elf --quiet
    DEPENDS image.elf
    USES_TERMINAL
)
