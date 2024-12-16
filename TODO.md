# TODO

- 优化一下普通布线函数在 bump 已经被连上 track 的时候的情况(track不用加，可以假设track不会被复用)
    begin_bump 和 end_bump 需要检查 connected_track ，如果不为 nullptr ，就需要找已经连好的路线；如果是 nullptr ，就按照普通布线流程走
    可以写一个模板：
    - 整个布线函数当中，用到 net 的只有获取 begin 和 end 的时候，可以把除 net 以外的内容整成模板
    - 
- 调整布线策略，在布线之间检查所有 net 里面对于 track 的限制，防止一条线布完之后增量布线的时候发现路径和目标不在一个 cobunit 里面
- 加一下 cobunit 的限制

- 测试布局
- GUI 在 linux 下无法移动的 BUG
- 补充文档
- Window 下输出乱码