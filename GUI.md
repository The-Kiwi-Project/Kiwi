# KIWI GUI

目前 interposer config 为空，可以把这些放到里面:
- COB 阵列的行、列
- TOB 的坐标
- VDD/GND 的外部端口位置

目前 topdie_inst 的创建和摆放位置位于同一个文件，可以拆分两个：

- 一个描述 topdieinst 名称与其对于 topdie 名称的 map（topdie_inst）
- 一个描述每个 topdieinst 名称与其放置位置的 map（layout）



启动 GUI

- 选择直接加载 config 文件运行，最后显示连接结果
    这样就相当于在 CLI 模式下运行，最后加一个显示连接结果的功能

- 选择自己绘制电路
    1. 导入 intersposer 配置
       这一步得到一个空的 Interspoer 对象

    2. 在原理图界面绘制电路和连接关系
       - 做一个类似电路库的功能，系统预先存储了例如 muyan（系统到指定路径下面读文件，形成若干个 TopDie 对象）的 TopDie
       - 可以导入自己的 TopDie（config 中的 topdies 中的描述）
       - 可以导入自己的 TopDie Inst（config 中的 topdie_insts 中的描述）
       - 可以导入外部端口文件（config 中的 external_ports 中的描述）
       - 可以导入电路连接关系（config 中的 connections.json）。导入会检查其中的所描述的 TopDieInst 是否存在。这样还可以继续在已存在的电路进行修改
       - 在电路原理图中,外部端口作为一个元件,可以拖动,连接。
    
       进入这一步时，系统先创建一个空的 BaseDie。之后慢慢填充这个对象。

       到这里到这里实际上和 CLI 导入的过程还是一样的。CLI 接下来会直接创建 Net 对象。但 GUI 不仅可以导入配置，也可以修改连接关系。
    
       由于 Config 中已经写好了从 String - String（即 ConnectionConfig）的连接关系描述到 Net 对象的转化。并且在转换为 Net 后丢弃所有的 ConnectionConfig。这使得导线信息展示十分困难。我想我们可以先将 ConnectionConfig 转为一个新的结构：

       ````rust
       enum Connection {
       	External(String),
           Pin(TopDieInst, String)
       }
       ````
    
       其实就是把 String 解析，如果是一个外部端口，那 String 不变。如果是 Bump（包含 xxx.xxx 结构），提取 TopDieInst 名字和引脚名，前者查 BaseDie 获得 TopDieInst 对象，后者保留。实际上 Config 中也是先做了这个处理，只不过没有保存下来！所以这个也好改。同时外部端口名称到位置的映射也丢失了。这个也需要保存到 BaseDie 中。
    
       那么现在我们就需要保存这个 Connection 列表到 BaseDie 中。也就是说 BaseDie 中有 `Vec<Connection>` 来保存连接的原始信息。也会有 Net 对象。
    
       所以在 GUI 中的实现：
    
       - 导入 TopDie 配置，向 BaseDie 添加新对象
       - 导入 TopDieInst 配置，向 BaseDie 添加新对象（因为还没有 Layout 信息，TopDieInst 的 tob 为空）
       - 导入 external_ports 配置，向 BaseDie 添加新对象。
       - 导入 connection 配置，这个文件被解析为 `Vec<Connection>`，并且显示到 GUI 上，但不会创建任何 Net，这个 `Vec<Connection>` 也不会被保存
       - 自己添加新的连接或在修改连接。只改变 GUI 中的绘制对象。总之，除了连接关系，其他的配置会直接导入到 basedie 对象中保存。
    
       当你确定原理图完成后。遍历所有的 GUI 连接，创建出一个 `Vec<Connection>`，保存到 BaseDie。之后 `Vec<Connection>` 就不可以被修改！
    
       所以到这里，系统中已经存在：Interposer 对象，包含了 Topdie、TopDieInst、外部端口信息以及连接关系的 BaseDie。
    
    3. 放置 TopdieInst
    
        此时所有 TopdieInst 的 tob 全为空。
    
        - 可以导入 layout 配置，直接更新 TopdieInst 的位置。
        - 可以直接在 GUI 上拖拽。
        - 随机生成 layout
    
        这一步结束后，系统中存在：Interposer 对象，包含了 Topdie、TopDieInst（带初始位置信息）、外部端口信息以及连接关系的 BaseDie。
    
        重用 config 中创建 Net 的过程，完成 Net 创建。
    
    4. 到这里就完成了输入的描述。进行布局布线
    
    5. 完成后,可以查看物理连接关系,支持 2D 和 3D 两种形式.
        - 2D: 可以查看每个连线的路径,支持具体每个硬件的详细展示
        - 3D: 只能查看全局连接,不支持修改连接



总结一下。我认为要修改的地方是：

- BaseDie：需要保存外部引脚信息 + Connection 信息
- Config：分离 topdieinst 对象和位置配置。稍微修改一下内部实现。可能需要暴露多一步 API（参加 Net 的）



## GUI 流程

进入软件需要用户选择一个模式：

1. 选择一个文件夹导入所有的电路信息。但不产生 Net 对象。即直接填充绘制原理图和摆放芯片两个窗口的内容。接下来可以对导入的电路进行修改。

    确定电路后执行布局布线，确定开始后将创建 Net 对象，电路配置不可以修改。

    完成后使用窗口展示结果。

2. 手动绘制电路。需要两个步骤：（1）绘制原理图、（2）摆放芯片初始位置。每个步骤需要一个不同的窗口实现。因为第二个窗口依赖第一个（摆放芯片依赖原理图，因为原理图包含系统存在哪些芯片、连接关系等），一旦第一个被修改，第二个窗口就需要更新才能保证显示正确（修改第二个不会影响第一个）。

    同样，确定两个窗口的内容（电路信息后），执行布局布线，确定开始后将创建 Net 对象，电路配置不可以修改。完成后使用窗口展示结果。

两个模式的区别是第一个直接导入了信息，第二个要自己绘制。但在确定电路信息后，都是一样的。

