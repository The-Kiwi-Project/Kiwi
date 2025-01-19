# TODO

- 输入文件里面的连接关系会重复写，读入的时候处理一下。
- 在 // route nets 外面再定义一个类包含布线与配置寄存器两步，然后将寄存器的 connect  从 route 里面拿出来单独进行，把路径单独存储
- 布线顺序控制一下
- 多端口线网布线最短的方式应该是斯坦纳树，但是这里不一定有必要
- mazereroute.cc 120行和144行在使用 optional 的 .value() 的时候没有检查，可能导致 Bad Optional Access 。一个track的connector是在前面还是后面

- 测试布局
- 布局部分的 GUI 
- 补充文档
