# **Route_Nets() 设计说明**

## **1. 调整 Net 优先级**

### **规则**：
- 原始优先级相同的 `net`：**端口数多的优先布线**。
- 原始优先级不同的 `net`：**保持优先级大小关系不变**。

### **实现步骤**：
1. **统计每个 `net` 的端口数量**。
2. **调用 `update_priority()`** 在原有优先级上附加一个值。
   - 原有优先级间隔为 **1**。
   - 附加值为 **(0, 0.9] 的浮点数**，不会破坏原有优先级大小关系。

---

## **2. Cobunit 限制的处理**

### **问题**：
- 一条路径上的 `track` 必须位于同一个 **cobunit** 上。
- **Bump** 选择 `available_tracks` 时需注意该限制。

### **实现步骤**：
1. 对于每条 `Bump->track` 或 `track->Bump` 的线：
   - 确定 `track` 所在的 `cobunit`。
   - 调用 `check_accessable_cobunit()` 将 `cobunit` 信息记录在 `bump` 的 `_accessable_cobunit` 中。

2. **初始化 `_accessable_cobunit`**：
   - 初始化为 `{1, 2, ..., 15}`。
   - **特殊情况**：
     - `Bump->Bump` 的线：无限制。
     - 含有一个或多个 `track` 的 `net`：
       - 遍历每个 `track`。
       - 获取其 `cobunit` 并与 `bump` 当前的 `_accessable_cobunit` 取交集，更新 `_accessable_cobunit`。

---

## **3. 资源管理：tob 上所有 bump 可用的 track**

### **问题**：
- **Bump->Bump** 的线：
  - 开始 `bump` 有很多 `available_tracks`。
- **Bump->Track** 的线：
  - **受 `cobunit` 限制**：一个 `cobunit` 最多 8 条 `tracks` 可连。
  - 这8条 `tracks` 可能被包含在前面 `bump->bump` 的 `available tracks` 里面，就有可能前面的很多 `bump->bump` 线， 明明有那么多 `tracks` 可用，就偏占了这 8 条，导致后面 `bump->track` 没有资源可以使用。

### **解决方案**：
1. 调用 `manage_cobunit_resources()`：
   - 根据每个 `tob` 上所有 `bump` 的 `_accessable_cobunit`，统计 `cobunit` 的需求情况。
   - 统计 0~15 每个 `cobunit` 的**使用总次数**。
   
2. 调用 `track_map_to_track_vec()`（在 `mazaroutestrategy.cc` 中）：
   - 按 `cobunit` 使用次数从少到多排序。一个 `cobunit` 使用次数越多，就越有可能同时被 `bump->bump` 和 `bump->track` 占用。
   - 返回的 `vector` 中，需求量少的 `cobunit` 上的 `tracks` 排在前面，被优先使用，减少后续竞争。

---

## **4. 路径长度计算 (`path_length`)**

### **问题**：
- 布线结果会出现一种情况，就是一条路径的某一段，多个连续的 `track` 连在同一个 `cobunit` 上，比如： 
   ```plaintext
  {row:1, col:1, hori, 0}->{row:2, col:1, vert, 0}->{row:1, col:2, hori, 0}
  ```
  这三条 `track` 全连在 `cob(1, 1)` 上，所以中间一条 `track` 其实只是当信号的中转点，而不是让信号从 `track` 的一端通向另一端，只有头尾的 `track` 是真正走信号的。我觉得这种中转 `track` 不能算一格路径。

### **解决方案**

#### **路径段规则**：
- 若路径中有 `>=3` 个连续 `track` 位于同一 `cobunit` 上，则该路径段的长度仅算 **2（头尾）**。

#### **逻辑实现**：
1. 在 `path_length` 函数的 `else` 模块中实现上述规则。
2. 使用变量 `switch_length` 控制：
   - 若规则不成立：
     - 将 `switch_length` 设为 **true**，简单统计所有 `track` 的数量作为路径长度。