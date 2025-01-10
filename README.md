# Kiwi

针对 [kiwimore](https://www.kiwimoore.com/) 设计的一款 chiplet interposer，所制作的布局布线工具。




## 项目结构

- [algorithm](./algorithm/)：算法开发与实验
- [document](./document/)：项目文档
- [resource](./resource/): GUI 资源
- [source](./source/)：项目源码
- [test](./test/)：项目模块测试
- [tools](./tools/)：工具程序




## 项目构建

本项目由 [xmake](https://github.com/xmake-io/xmake) 工具构建。

安装xmake：[xmake安装](https://xmake.io/mirror/zh-cn/guide/installation.html)

保证系统安装 xmake 以及支持 C++20 版本的编译器。在根目录调用：

````bash
xmake build kiwi
````



## 命令行参数

````bash
kiwi <input folder path> [OPTIONS]
````


Options：
- `-o, --output <OUTPUT_PATH>`：指定输出 controlbit 文件路径
- `-g, --gui`：使用 GUI 模式
- `-h, --help`：打印帮助信息
- `-V, --version`：打印版本信息
- `-v, --verbose`: 输出 Debug 信息



## 工具程序

tools 目录下保存一些实用的工具小程序：

- [cobmap](./tools/cobmap.cc)：计算 COB 端口信息的映射关系
- [view2d](./tools/view2d.cc)：对指定 config 进行布线，使用 2D 显示布线结果
- [view3d](./tools/view3d.cc)：对指定 config 进行布线，使用 3D 显示布线结果

根据 xmake.lua 文件中的命令，执行 `xmake build <tool>` 构建、`xmake run <tool> [args]` 允许相应的工具。除此，还提供了 count_lines.py 来计算源代码总行数。



## 测试

test 目录下保存项目的测试代码，分为 module_test 和 regression_test 两个部分。

### module_test

对各个模块进行单独测试。

编译：

```bash
xmake build module_test
```

运行：

````bash
xmake run module_test [module]
````

`module` 指定要测试的模块（见 simpletest 目录），`all` 表示全部测试。

### regression_test

集成系统各个模块，用于回归测试，目前包含 16 个 case 。
- case 1-6 测试基本的功能 ：
    |case|说明|
    |:---:|:---:|
    |[case1](./test/config/case1)|仅测试同步线布线功能|
    |[case2](./test/config/case2)|测试同步线，并对同步线中的一个bump增加一根连到I/O的线，测试已布线bump的复用|
    |[case3](./test/config/case3)|仅测试非同步线布线功能|
    |[case4](./test/config/case4)|包含 VDD/GND|
    |[case5](./test/config/case5)|重复连接|
    |[case6](./test/config/case6)|更多的连接数量|
- case 7-16 使用构造的芯粒系统进行测试，系统中包含所有类型的线网与较多的线网数量 ：
    |case|说明|
    |:---:|:---:|
    |case 7-9|一个 cpu-ai-mem 芯粒系统|
    |case 10-12|一个 cpu 芯粒系统|
    |case 13-16|一个 AI core 芯粒系统|

编译：

```bash
xmake build regression_test
```

运行：

```bash
xmake run regression_test
```
