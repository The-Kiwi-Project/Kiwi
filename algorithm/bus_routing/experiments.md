## 徐磊的代码
  - routing.cpp 布线的流程
    布线初始化的时候，有一个trackdetail存了interposer上面所有的track。布线的时候使用一个 vector 来记录搜索过程中走过的所有路径.
    拆线重布的过程在函数 tryFindSinksSync() 里面。首先有一个长度上限，如果当前布线长度大于已经记录的最大长度的2倍，则直接停止。检测一个 track 是不是与其祖先节点重合，使通过“路径树”向上查找的方式完成

## 关于算法
1. 试了一些例子，发现资源冲突方面对于布线成功率的影响比较大，还有布线顺序也有影响。主要的冲突在于 TOB 上的 bump 与下方的 track 的连接途径不太够用
2. 这里的同步线，如果两条线长度相差奇数，是可以调成功的。
3. 先预测线长再布线？?


## TODO
1. 初步尝试发现增大重布线的时候的拆线比例可用提高重连成功率，但是线长会增加。可做一下实验，用case12



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
    1) route each net by maze routing and mark those occupied points
    2) find the largest net length, marked as `max_length`
    3) find all nets which has length less than `max_length`, and store them in a list `to_be_routed`
    4) set the `rate` of removing tracks as 0.2, that is to keep 80% of the tracks near the start point and remove the rest. 
       `For each net`, remove the 20% tracks and clear related marks in `grid`
    5) in the `for loop` in 4)., create a new `tree`, with the start point(the end of the partially-cut net) being the root node
    6) also in the `for loop` in 4)., enter `an endless loop`(a method to substrate BFS searching):
        - get `wavefront = tree.leaves`
        - if `wavefront` is empty, then routing failed
        - else, going through each `leaf` in `wavefront`:
            if the current `leaf` is `end`, then backtrace and get the rerouted_path:
              if `length(the partially-cut-net + rerouted_path) == max_length`, then mark the new path in `grid` and `continue` to the    
                next net in the `for loop` in 4).
              if `length(the partially-cut-net + rerouted_path) > max_length`, then mark the new path in `grid`, update the `max_length` 
                and go back to 3). to find those nets to be rerouted
              if `length(the partially-cut-net + rerouted_path) < max_length`, then remove the current `leaf` in `tree.leaves` and 
                `continue ` the `endless loop` in 6). 
            else, check whether the `leaf`'s neighbours are legal and are not predecessors of the `leaf`, then add them to 
            `leaf.children`
        - in 4), if all nets are rerouted and keep the length same, then end the algothrim

- **关于 FPGA　和　GRID 的区别**
　GRID当中每个点直接与相邻的格子连接，可以看成一张节点度为４的无向图
　FPGA当中，根据Wilton结构，每个点和６个点连接，可以看成一张节点度为６的无向图　

  打算在GRID上做算法和可视化，然后移植到FPGA上检查结果


- **测试:GRID**
  1. 在 EX1 中可以得到正确的结果
    nets： [[(1, 5), (10, 13)], [(3, 8), (13, 11)], [(6, 1), (19, 3)]]
    maze: 20*20 grid
  
  2. 在 EX3 中也可以正常布通
    nets = [[(1, 5), (14, 13)], [(3, 8), (13, 11)], [(6, 1), (19, 5)]]
    maze = 20*20 grid

    感觉这是一种保底的方法，确实可以找到最短路径，但是时间和空间复杂度都很大。相当于迷宫算法一定可以找到一条最短路径，但是效率较低。
    想找一找那些单根线快速布线的方法，尤其是对迷宫算法是如何优化的
  
  3. EX4
    nets = [[(1, 5), (14, 13)], [(3, 8), (20, 20)]]
    maze = 50*50 grid

    先用原来的算法跑了一遍，结果见bus_routing.log
  
  4. EX5
    还是EX4的设置

    修改：在重布线的时候加入A*，把原来用leaves一层一层扩展的思路换掉，还用队列实现
    segmentation_bus_routing_1
    快了很多
  
  5. EX7
    实验设置同 EX3
    在搜索的时候加入A*，预估的距离就用当前位置和目标位置的直角距离计算，并结合优先队列实现搜索


- **测试:FPGA**
  注：优化一下代码
