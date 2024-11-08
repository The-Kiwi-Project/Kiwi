## 徐磊的代码
  - routing.cpp 布线的流程
    布线初始化的时候，有一个trackdetail存了interposer上面所有的track。布线的时候使用一个 vector 来记录搜索过程中走过的所有路径.
    拆线重布的过程在函数 tryFindSinksSync() 里面。首先有一个长度上限，如果当前布线长度大于已经记录的最大长度的2倍，则直接停止。检测一个 track 是不是与其祖先节点重合，使通过“路径树”向上查找的方式完成




## 实验设置

### 实验一、 先复现徐磊的算法，看一下主要耗时和占用存储空间的地方在哪里
    
#### Version1 根据论文的算法，可以完成同步线的长度相等的要求

- **算法**
  **Data structure:**
  1. **Class Node**
      - attributes
        - `coordinate`: (x, y)
        - `parent`
        - `children`: a list

      - methods    
        - `add_child()`: to add a child node in the children list
        - `remove_child()`: to remove a child node in the children list
        - `get_parent()`: to get the parent node

  2. **Class Tree**
      - attributes
        - `root`
        - `leaves`: a list
        > notice that root.parent = None

      - methods    
        - `update_leaves()`: to update the leaves list, by going through the tree from the root node and adding the node with no 
      children to the leaves list
        - `is_a_predecessor()`: to check if a node is a predecessor of its father, by moving toward the fathers through  
      method "get_parent"
        > notice that checking if a node is a predecessor of its father is different from checking if a node is visited before in the 
          simple BFS search algorithm
  
  **Algorithms**
  1. **`maze_routing(grid, net)`**
     simple routing algorithm, using BFS to find the shortest path from start to end

  2. **`bus routing(grid, nets)`**
  > from xl's paper, with more details in the code
    1) route each net by maze routing and mark those occupied point
    2) find the largest net length, marked as `max_length`
    3) find all nets which length is less than `max_length`, and store them in a list `to_be_routed`
    4) set the `rate` of removing tracks as 0.2, that is to keep 80% of the tracks near the start point. 
       `For each net`, remove the tracks and clear related marks in `grid`
    5) in the `for loop` in 4)., create a new `tree`, with the start point(the end of the partially-cut net) as the root node
    6) also in the `for loop` in 4)., enter `an endless loop`(to substrate BFS searching):
        - get `wavefront = tree.leaves`
        - if `wavefront` is empty, then routing failed
        - else, going through each `leaf` in `wavefront`:
            if the current `leaf` is `end`, then backtrace and get the rerouted_path:
              if `length(the partially-cut-net + rerouted_path) == max_length`, then mark the new path in `grid` and `continue` to the    
                next net in the `for loop` in 4).
              if `length(the partially-cut-net + rerouted_path) > max_length`, then mark the new path in `grid`, update the `max_length` 
                and go back to 3). to refind those nets to be rerouted
              if `length(the partially-cut-net + rerouted_path) < max_length`, then remove the current `leaf` in `tree.leaves` and 
                `continue ` the `endless loop` in 6). 
            else, check whether the `leaf`'s neighbours are illegal and are not predecessors of the `leaf`, then add them to 
            `leaf.children`
        - in 4), if all nets are rerouted and keep the length same, then end the algothrim


- **测试**
  1. 在以下情况中可以得到正确的结果
    - nets： [[(1, 5), (10, 13)], [(3, 8), (13, 11)], [(6, 1), (19, 3)]]
    - maze: 20*20
       


- 问题：
  1. 不同的布线顺序会影响布通性

    - 改例子无法布通
      nets = [[(1, 5), (10, 9)], [(2, 7), (40, 25)], [(3, 8), (20, 30)], [(4, 6), (30, 40)]]
      maze = np.zeros((50, 50))
