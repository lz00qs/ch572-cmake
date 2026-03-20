# ch572-cmake

一个基于 CMake + Ninja 的 WCH CH572 工程模板，已经集成了：

- 启动文件和链接脚本
- CH57x 标准外设驱动
- BLE 预编译库
- USB 相关库
- Windows / Unix 风格构建脚本

当前 `src/main.c` 是一个 UART0 收发示例：上电后先发送一段字符串，然后进入串口回显循环。

## 目录结构

```text
.
|-- BLE_LIB/            BLE 相关头文件和静态库
|-- Ld/                 链接脚本
|-- RVMSIS/             RISC-V 内核相关头文件
|-- Startup/            启动文件
|-- StdPeriphDriver/    CH57x 标准外设驱动和静态库
|-- USB_LIB/            USB 相关源码和静态库
|-- src/                用户代码
|-- CMakeLists.txt      主构建脚本
|-- CMakeEnvs.txt       当前使用的构建环境配置
|-- build.bat           Windows 构建脚本
|-- build.sh            Unix 构建脚本
|-- clean.bat           Windows 清理脚本
|-- clean.sh            Unix 清理脚本
```

## 环境要求

建议准备以下工具：

- CMake 3.20 或更高
- Ninja
- WCH RISC-V GCC 工具链

这个工程当前默认按 WCH / MounRiver Studio 的工具链路径来配置。仓库里的默认配置示例使用的是：

```text
C:/MounRiver/MounRiver_Studio2/resources/app/resources/win32/components/WCH/Toolchain/RISC-V Embedded GCC12/bin/riscv-wch-elf-
```

注意：

- `build.bat` 和 `build.sh` 会直接调用 `cmake` 和 `ninja`
- 如果你的终端里这两个命令不在 `PATH` 中，构建会失败
- 我在当前环境里验证时就遇到了 `cmake` 未找到的问题，所以 README 这里特别说明

## 配置工具链

工程通过 `CMakeEnvs.txt` 读取工具链和项目配置，格式如下：

```text
TOOLCHAIN=C:/path/to/riscv-wch-elf-
TOOLCHAIN_INCLUDE=C:/path/to/riscv-wch-elf/include
PROJECT_NAME=ch572-cmake
CMAKE_BUILD_TYPE=debug
```

字段说明：

- `TOOLCHAIN`: 编译器前缀，不包含 `gcc.exe` 本体，例如 `.../bin/riscv-wch-elf-`
- `TOOLCHAIN_INCLUDE`: 工具链自带头文件目录
- `PROJECT_NAME`: 最终生成文件名
- `CMAKE_BUILD_TYPE`: 可选 `debug` 或 `release`

仓库中还提供了：

- `CMakeEnvs-Windows.txt`
- `CMakeEnvs-Mac.txt`

你可以按自己的平台修改后，再覆盖到 `CMakeEnvs.txt` 使用。

## 构建方法

### Windows

```powershell
.\build.bat
```

### macOS / Linux

```bash
sh build.sh
```

脚本会：

1. 删除并重建 `build/`
2. 在 `build/` 中执行 `cmake -GNinja ..`
3. 调用 `ninja` 开始编译

如果你想手动构建，也可以执行：

```powershell
mkdir build
cd build
cmake -GNinja ..
ninja
```

## 构建产物

编译成功后，`build/` 目录下会生成：

- `${PROJECT_NAME}.elf`
- `${PROJECT_NAME}.hex`
- `${PROJECT_NAME}.bin`
- `${PROJECT_NAME}.map`

其中：

- `elf` 适合调试
- `hex` / `bin` 适合烧录
- `map` 用于查看链接结果和内存占用

链接阶段已经开启了内存使用统计和无用段裁剪。

## 示例程序说明

当前示例位于 `src/main.c`，主要做了这些事情：

1. 关闭两线调试接口
2. 配置外部高速晶振和系统时钟到 100MHz
3. 将 UART0 重映射到 `PA3(TX)` / `PA2(RX)`
4. 发送字符串 `This is a tx exam`
5. 进入轮询接收并回显

如果你接串口测试，注意串口引脚映射和外设初始化是否符合你的硬件连接。

## 烧录说明

这个仓库当前没有集成烧录命令或 OpenOCD / WCHISP 脚本，通常做法是：

- 使用 MounRiver Studio 的下载工具烧录 `build/*.elf` 或 `build/*.bin`
- 或使用你现有的 WCH 烧录工具链进行下载

如果你后续想把“构建 + 烧录”也一起接入 CMake，可以再补一个下载目标。

## 常见问题

### 1. `cmake` 或 `ninja` 找不到

把 CMake 和 Ninja 加到系统 `PATH`，或者使用带这些工具的开发终端。

### 2. 工具链路径不对

先检查 `CMakeEnvs.txt` 里的：

- `TOOLCHAIN`
- `TOOLCHAIN_INCLUDE`

这两个路径必须和你本机安装位置一致。

### 3. 注释中文乱码

仓库里有一个 `recoding.py`，用途是把部分源码从 `GB2312` 转成 `UTF-8`。如果你看到驱动或示例里的中文注释乱码，可以按需处理相关文件编码。

## 后续建议

如果准备把这个仓库长期作为模板使用，比较值得继续补充的内容有：

- 烧录脚本
- 串口监视脚本
- `Debug` / `Release` 的更清晰区分
- 针对 CH572 板卡的默认引脚和硬件说明
