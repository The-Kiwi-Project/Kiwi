# TODO

- 输入文件里面的连接关系会重复写，读入的时候处理一下
- 一个 tob 的所有资源都是共用的，应该优先考虑目标 track 选项较少的 bump 。可以统计对于 cobunit 的需求，然后在布线的时候先从需求少的 cobunit 开始用.
  需要把 begin_tracks 改成 set ，考虑元素顺序
  让 tob 统计所有 bump 的cobunit 使用情况，然后根据使用频率做一个排序。在把 begin_tracks_map -> begin_tracks_set 的时候，改成用 ordered_set ，然后把使用频率低的放前面

- 测试布局
- GUI 在 linux 下无法移动的 BUG
- 补充文档
- Window 下输出乱码