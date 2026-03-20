set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
cmake_minimum_required(VERSION 3.20)

# 读取环境变量配置
file(READ "CMakeEnvs.txt" env_file_contents)
# 分行处理文件内容
string(REPLACE "\n" ";" env_file_lines "${env_file_contents}")
foreach(line ${env_file_lines})
  # 匹配 KEY=VALUE 格式
  string(REGEX MATCH "^(.*)=(.*)$" match ${line})
  if(match)
    set(${CMAKE_MATCH_1} ${CMAKE_MATCH_2})
  endif()
endforeach()
message(STATUS "TOOLCHAIN: ${TOOLCHAIN}")
message(STATUS "PROJECT_NAME: ${PROJECT_NAME}")

if (WIN32)
    MESSAGE(STATUS "Now is windows!")
    set(CMAKE_C_COMPILER ${TOOLCHAIN}gcc.exe)
    set(CMAKE_CXX_COMPILER ${TOOLCHAIN}g++.exe)
    set(CMAKE_ASM_COMPILER ${TOOLCHAIN}gcc.exe)
    set(CMAKE_AR ${TOOLCHAIN}ar.exe)
    set(CMAKE_OBJCOPY ${TOOLCHAIN}objcopy.exe)
    set(CMAKE_OBJDUMP ${TOOLCHAIN}objdump.exe)
    set(SIZE ${TOOLCHAIN}size.exe)
elseif (UNIX)
    MESSAGE(STATUS "Now is UNIX-like OS!")
    set(CMAKE_SYSTEM_NAME Linux)
    set(CMAKE_C_COMPILER ${TOOLCHAIN}gcc)
    set(CMAKE_CXX_COMPILER ${TOOLCHAIN}g++)
    set(CMAKE_ASM_COMPILER ${TOOLCHAIN}gcc)
    set(CMAKE_AR ${TOOLCHAIN}ar)
    set(CMAKE_OBJCOPY ${TOOLCHAIN}objcopy)
    set(CMAKE_OBJDUMP ${TOOLCHAIN}objdump)
    set(SIZE ${TOOLCHAIN}size)
else ()
    MESSAGE(STATUS "Unsupported system!")
endif ()

set(CMAKE_SYSTEM_NAME Generic)

# 项目名称以及语言
project(${PROJECT_NAME} C CXX ASM)

# 设置编译标准
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)

# 设置编译选项
add_compile_options(
    -march=rv32imc_zba_zbb_zbc_zbs_xw
    -mabi=ilp32
    -mcmodel=medany
    -msmall-data-limit=8
    -mno-save-restore
    -fmax-errors=20
    -Os
    -fmessage-length=0
    -fsigned-char
    -ffunction-sections
    -fdata-sections
    -fno-common
    --param=highcode-gen-section-name=1
    -g
    $<$<COMPILE_LANGUAGE:C>:-std=gnu99>
    $<$<COMPILE_LANGUAGE:ASM>:-x>
    $<$<COMPILE_LANGUAGE:ASM>:assembler-with-cpp>
)

if (CMAKE_BUILD_TYPE STREQUAL "debug")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall -O0") # 禁用优化
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
    set(CMAKE_ASM_FLAGS_DEBUG "-O0 -g")
    add_definitions(-DDEBUG=Debug_UART0)
    message("Debug mode:${CMAKE_C_FLAGS_DEBUG}")
else()
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Wall -O3") # 启用优化
    set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -Os") # 优化空间
    set(CMAKE_ASM_FLAGS_RELEASE "-DNDEBUG -Os")
    message("Release mode:${CMAKE_C_FLAGS_RELEASE}")
endif()

# Include directories
include_directories(
    ${TOOLCHAIN_INCLUDE}
    src/include
    BLE_LIB
    RVMSIS
    StdPeriphDriver/inc
    USB_LIB)

# Source files
file(
    GLOB_RECURSE
    SOURCES
    "src/*.c"
    "RVMSIS/*.c"
    "Startup/*.S"
    "StdPeriphDriver/*.c"
    "USB_LIB/*.c"
)

# 添加 link map 文件
set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/Ld/Link.ld)
add_link_options(
    -march=rv32imc_zba_zbb_zbc_zbs_xw
    -mabi=ilp32
    -mcmodel=medany
    -msmall-data-limit=8
    -mno-save-restore
    -fmax-errors=20
    -Os
    -fmessage-length=0
    -fsigned-char
    -ffunction-sections
    -fdata-sections
    -fno-common
    --param=highcode-gen-section-name=1
    -g
    -T${LINKER_SCRIPT}
    -nostartfiles
    -Xlinker
    --gc-sections
    -Xlinker
    -Wl,--print-memory-usage,-Map,${PROJECT_NAME}.map
    --specs=nano.specs
    --specs=nosys.specs
)

# 生成可执行文件
add_executable(${PROJECT_NAME}.elf ${SOURCES})

# Link the static libraries
target_link_libraries(
    ${PROJECT_NAME}.elf
    ${CMAKE_SOURCE_DIR}/StdPeriphDriver/libISP572.a
    ${CMAKE_SOURCE_DIR}/BLE_LIB/libCH572BLE_PERI.a
    ${CMAKE_SOURCE_DIR}/USB_LIB/libRV3UFI.a)

# Generate binary and hex files
set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)
add_custom_command(
    TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
    COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE})