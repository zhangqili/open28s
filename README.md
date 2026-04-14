# Open28S

适配28S Hitbox的开源固件

## 构建

### 前置条件
- CMake
- Ninja
- gcc(Windows参考<https://code.visualstudio.com/docs/cpp/config-mingw>搭建MinGW环境，需要将`C:\msys64\usr\bin`与`C:\msys64\ucrt64\bin`依次放置在系统Path环境变量的最上方)
- 在ST官网上下载安装STM32Cube开发套件 <https://www.st.com.cn/zh/development-tools/stm32cubeclt.html>
- 在ST官网上下载安装STM32CubeProgrammer <https://www.st.com.cn/zh/development-tools/stm32cubeprog.html>，安装STM32Bootloader驱动
- 将该仓库克隆到本地并在该仓库文件夹打开命令行终端
```PowerShell
git clone --recurse-submodules https://github.com/zhangqili/open28s.git
cd oholeo-keyboard-firmware
```


### 设置环境变量
以STM32CubeCLT 1.17.0为例，该开发套件默认安装至C:\ST\STM32CubeCLT_1.17.0，接下来你需要将开发套件中的编译器目录添加至环境变量

以PowerShell为例
```PowerShell
$Env:PATH += ";C:\ST\STM32CubeCLT_1.17.0\GNU-tools-for-STM32\bin"
```

### 运行编译命令
```PowerShell
mkdir build
cd build
cmake .. -G"Ninja" --preset=Release
cd Release
ninja
```

## 烧录

### SWD
将STLink连接至PCB的SWD接口，打开STM32CubeProgrammer选择ST-LINK进行烧录
