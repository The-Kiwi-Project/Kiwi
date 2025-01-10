# Update

1. 原理图 GUI 基本完成，可以对一个 basedie 对象进行编辑：
    - 导入新 TopDie
    - 放置、移动、删除 TopDieInstance
    - 连接、删除导线
    - 添加、移动、修改外部端口
    修改过程中，basedie 对象中的信息实时更新

2. 在 BaseDie 类中添加 ExternalPort 以及 Connection 记录电路信息

3. 在 BaseDie 类中添加部分编辑电路信息的 API

4. 修改 reader.cc，将创建 Net 对象的步骤转移到 algo 模块，需要在布线前单独调用（也许可能将 Net 直接放在 algo 模块中）！！！

