# 配置文件说明

config.json 指出对应配置文件的路径：

- interposer：Interposer 信息
- topdies：TopDie 的信息。TopDie 名称 ：引脚映射
- topdie_insts：TopDie 的实例化信息。TopDie 实例名称 ：其所属的 TopDie + 初始 TOB 位置
- external_ports：外部端口信息。端口名称 ： Track 位置
- connections：连接关系。外部端口直接使用名称，TopDie 引脚使用 topdie_inst.pin_name 的形式给出
- reigster_adders：寄存器地址。





注：现在的程序和 xl 程序使用的坐标体系相比，y不变，x = 9-x。
    所以如果是读 excel ，excel是按照 xl 的坐标来的，需要把 I/O 那一个 sheet 第二列的三个数当中中间一个数 x 换成 9-x 。然后第一个数是方向，0对于 xl 的来说是 hori ，我们这里需要写成 vert
    第三列那个数值是个索引，先从 xl 那里找到 I/O 可用端口的序列，然后用索引在序列中找到具体的端口号

