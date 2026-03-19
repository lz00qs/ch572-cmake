@echo off

REM 检查 build 目录是否存在
if exist build (
    echo Found build/
    rmdir /s /q build
    mkdir build
) else (
    echo Not Found build/
    mkdir build
)

REM 进入 build 目录
cd build

REM 执行 CMake 和 Ninja 构建
cmake -GNinja ..
ninja