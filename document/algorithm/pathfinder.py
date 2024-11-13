# pathfinder 

import heapq

# 每次布线的时候，一个 node 可以被重复使用，所以只要每次记录自己的 routing tree ，其它的 tree 不用管
# 给所有点编号，图用邻接矩阵存，然后初始化一个字典，编号为键，值是 node ，以此记录全局所有 node 的信息

class Node:
    def __init__(self, index) -> None:
        self.shared_cost = 0
        self.history_cost = 0
        self.index = index


class Graph:
    def __init__(self, adj, node_num) -> None:
        # 用邻接矩阵存储图及其各条边的权重
        # max_node_num 是最大的点的个数，但是点的编号从0开始

        self.adj = adj
        self.max_node_num = node_num
        self.shared_dic = {}


def checkShared(shared_dic: dict) -> bool:
    # 检查是否还有点被共享，有就返回 True

    for value in shared_dic.values():
        if value.shared_cost > 1:
            return True
    return False


def backtrace(predecessor, target) -> list:
    # 回溯得到一条路径（不含起点）

    partial_path = []
    while predecessor[target] != -1:
        partial_path.insert(0, target)
        target = predecessor[target]
        
    return partial_path 



def pathfinder(graph: Graph,  # 一张图
            S_and_T: dict     # 一个字典，键是起点编号，值是一个 list ，存放所有终点的编号
            ) -> dict:
    trees = {}
    for key in S_and_T.keys():
        trees[key] = []   # 初始化一棵树
        graph.shared_dic[key] = Node(key)

    while True:
        for source, targets in S_and_T.items():
            copy_T = targets[:]
            path = [source]
            
            while not len(copy_T) == 0:
                # 初始化 wavfront 和 predecessor
                predecessor = [-1 for i in range(graph.max_node_num)]
                wavefront = []
                for s in path:
                    wavefront.append((0, s))
                heapq.heapify(wavefront)

                while True:
                    if len(wavefront) == 0:
                        print(f"failed finding a path from the source node {source}")
                        exit()
                    
                    curr_cost, curr_num = heapq.heappop(wavefront)
                    if curr_num in copy_T:
                        partial_path = backtrace(predecessor, curr_num)
                        path += partial_path
                        copy_T.remove(curr_num)
                        break


                    adj_row = graph.adj[curr_num]
                    assert len(adj_row) == graph.max_node_num, "邻接矩阵维度错误"
                    for k in range(graph.max_node_num):
                        arc = adj_row[k]
                        if arc != 0:
                            # 记录前驱
                            predecessor[k] = curr_num

                            # 记录该点被用过
                            if k not in graph.shared_dic.keys():
                                graph.shared_dic[k] = Node(k)

                            # 加入最小堆
                            bn = arc+curr_cost
                            hn = graph.shared_dic[k].history_cost
                            pn = graph.shared_dic[k].shared_cost
                            cn = (bn + hn) * pn
                            heapq.heappush(wavefront, (cn, k))

            # 更新某个起点对应的树
            tree = trees[source]
            for n in tree:
                graph.shared_dic[n].shared_cost -= 1
            for n in path:
                graph.shared_dic[n].shared_cost += 1
                graph.shared_dic[n].history_cost += 1
            trees[source] = path

        if not checkShared(graph.shared_dic):
            break
    
    return trees
        

if __name__ == "__main__":
    adj = [
           [0, 0, 0, 2, 1, 0, 0, 0, 0],
           [0, 0, 0, 3, 1, 4, 0, 0, 0],
           [0, 0, 0, 0, 1, 3, 0, 0, 0],
           [0, 0, 0, 0, 0, 0, 2, 3, 0],
           [0, 0, 0, 0, 0, 0, 1, 1, 1],
           [0, 0, 0, 0, 0, 0, 0, 4, 3],
           [0, 0, 0, 0, 0, 0, 0, 0, 0],
           [0, 0, 0, 0, 0, 0, 0, 0, 0],
           [0, 0, 0, 0, 0, 0, 0, 0, 0]
          ]
    max_node_num = 9
    graph = Graph(adj, max_node_num)
    S_and_T = {
        0: [6],
        1: [7],
        2: [8]
    }
    rT = pathfinder(graph, S_and_T)
    for key, value in rT.items():
        print(f"source={key}, Path:")
        print(value)
        print("")









