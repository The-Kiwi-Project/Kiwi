# TODO

- 输入文件里面的连接关系会重复写，读入的时候处理一下。
- 怎么处理布线失败。还有几个 case 布线还是失败的
- mazereroute.cc 120行和144行在使用 optional 的 .value() 的时候没有检查，可能导致 Bad Optional Access 。一个track的connector是在前面还是后面

- 测试布局
- 布局部分的 GUI 
- 补充文档
