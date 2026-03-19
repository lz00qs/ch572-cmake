@echo off

REM 检查 build 目录是否存在
if exist build (
    echo Found build/, removing...
    rmdir /s /q build
    echo Done.
) else (
    echo Not Found build/, it seems the build files are already deleted!
)