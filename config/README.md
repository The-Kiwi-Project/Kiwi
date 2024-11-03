# 配置文件说明

config.json 指出对应配置文件的路径：

- interposer：Interposer 信息
- topdies：TopDie 的信息。TopDie 名称 ：引脚映射
- topdie_insts：TopDie 的实例化信息。TopDie 实例名称 ：其所属的 TopDie + 初始 TOB 位置
- external_ports：外部端口信息。端口名称 ： Track 位置
- connections：连接关系。外部端口直接使用名称，TopDie 引脚使用 topdie_inst.pin_name 的形式给出
- reigster_adders：寄存器地址。
