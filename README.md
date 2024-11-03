# Kiwi

针对 [kiwimore](https://www.kiwimoore.com/) 设计的一款 chiplet interposer，所制作的的布局布线工具。




## 项目结构

- [config](./config/)：输入配置文件
- [document](./document/)：项目文档
- [source](./source/)：源码
- [test](./test/)：项目模块测试
- [algorithm](./algorithm/)：算法实现
- [script](./script/)：辅助脚本




## 编译与运行

本项目由 [xmake](https://github.com/xmake-io/xmake) 工具构建。

保证系统安装 xmake 以及支持 C++20 版本的编译器。在根目录调用：

````bash
xmake build kiwi
````

即可编译整个项目。再执行以下命令运行：

````bash
xmake run kiwi
````

