# Kiwi

针对 [kiwimore](https://www.kiwimoore.com/) 设计的一款 chiplet interposer，所制作的布局布线工具。




## 项目结构

- [algorithm](./algorithm/)：算法开发与实验
- [config](./config/)：输入配置文件
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
- `-v, --version`：打印版本信息




## 测试

test 目录下保存项目的测试代码，分为 simpletest 和 regressiontest 两个部分
- simpletest 用于手动构造简单的测例，快速测试
- regressiontest 用于做回归测试


编译：
(注： regressiontest 还没有实现)

````bash
xmake build simpletest
````

运行测试：

````bash
xmake run simpletest [module]
````

`module` 指定要测试的模块（见 simpletest 目录），`all` 表示全部测试

