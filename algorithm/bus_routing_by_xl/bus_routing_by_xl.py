import numpy as np 
import heapq
import matplotlib.pyplot as plt
from matplotlib.colors import ListedColormap
from matplotlib import cm
from routing_tree import *
import os


def visualize(maze, title, nets, savedir):
    num_classes = int(maze.max()+1)
    colors = cm.get_cmap("tab20", num_classes)
    cmap = ListedColormap([colors(i) for i in range(num_classes)])
    plt.imshow(maze, cmap=cmap)
    plt.grid(color='gray', linestyle='-', linewidth=0.5)
    plt.xticks(np.arange(-0.5, len(maze[0]) - 0.5, 1), [str(i) for i in range(len(maze[0]))], fontsize=8)
    plt.yticks(np.arange(-0.5, len(maze) - 0.5, 1), [str(i) for i in range(len(maze))], fontsize=8)

    plt.gca().xaxis.set_ticks_position('top')
    plt.gca().tick_params(axis='x', rotation=45)

    plt.colorbar(ticks=[i for i in range(num_classes)], label='maze Values')  

    for i, net in enumerate(nets):
        for point in net:
            plt.text(point[1], point[0], str(i), ha="center", va="center", color="black", fontsize=8)

    plt.title(title)

    savepath = f"{savedir}/{title}"
    plt.savefig(savepath, dpi=300, bbox_inches='tight')

    plt.show()
    

def collect_result(maze, nets, logger):
    for i in range(len(nets)):
        count = np.count_nonzero(maze == (2+i))
        logger.info(f"net {i} has length={count}")


def backtrace(visited, end):
    '''
    backtrace the routing path, not including the start point   
    '''

    path = []
    while visited[end] != "B":
        path.insert(0, end)
        if visited[end] == "S":
            end = (end[0] + 1, end[1])
        elif visited[end] == "N":
            end = (end[0] - 1, end[1])
        elif visited[end] == "E":
            end = (end[0], end[1] + 1)
        elif visited[end] == "W":
            end = (end[0], end[1] - 1)
    
    return path


def maze_routing(maze, nets, start, end, logger, experiment_dir, color_mapper = 0):
    '''
    routing a net from start to end

    maze: a 2D numpy array, 0 for free space, 1 for obstacle
    start: a tuple, (x, y)
    end: a tuple, (x, y)
    return: routed path & maze
    '''
    max_x, max_y = maze.shape
    visited = np.full((max_x, max_y), " ")
    visited[start] = "B"
    maze[start] = 2 + color_mapper
    queue = [(0, start)]

    while queue:
        cost, (x, y) = heapq.heappop(queue)
        
        if (x, y) == end:
            path = backtrace(visited, end)
            for p in path:
                maze[p] = 2 + color_mapper
            return path, maze
        
        neighbours = [[(-1, 0), "S"], [(1, 0), "N"], [(0, -1), "E"], [(0, 1), "W"]]
        for (dx, dy), direc in neighbours:
            nx, ny = x + dx, y + dy
            if 0 <= nx < max_x and 0 <= ny < max_y and maze[nx, ny] == 0 and visited[nx, ny] == " ":
                visited[nx, ny] = direc
                heapq.heappush(queue, (cost + 1, (nx, ny)))
    
    logger.error(f"Routing failed")
    visualize(maze, "routing_failed", nets, experiment_dir)
    print("Routing failed")
    exit()


def remove_tracks(maze, path, rate):
    '''return cleared path and the end point'''

    l = len(path)
    cut_length = int(l * rate) if int(l * rate) > 0 else 1
    for coord in path[cut_length:]:
        maze[coord] = 0

    return path[:cut_length], path[-1]


def bus_routing(maze, nets, logger, experiment_dir, rate = 0.8):
    '''
    routing all nets in the maze

    maze: a 2D numpy array, 0 for free space, 1 for obstacle
    nets: a list of tuples, each tuple is (start, end)
    return: routed path & maze
    '''

    def reroute(to_be_routed, max_length, paths):
        def reroute_single_net(tree, max_length):
            while True:
                wavefronts = tree.leaves
                if not wavefronts:
                    logger.error(f"routing failed for net {i}")
                    raise ValueError(f"routing failed for net {i}")
                else:
                    for current_node in wavefronts:
                        current_coord = current_node.coord
                        current_cost = current_node.cost

                        if current_coord == end:
                            re_routed_path = tree.backtrace(current_node)
                            if len(re_routed_path) + len(cleared_path) > max_length:
                                paths[i] = cleared_path + re_routed_path
                                max_length = len(paths[i])
                                for p in paths[i]:
                                    maze[p] = 2 + i
                                return False, max_length # 最大长度有变
                            elif len(re_routed_path) + len(cleared_path) == max_length:
                                paths[i] = cleared_path + re_routed_path
                                for p in paths[i]:
                                    maze[p] = 2 + i
                                return True, max_length # 最大长度没变           
                            else:
                                tree.remove_leaf(current_node)
                                continue
            
                        for dx, dy in [(0, 1), (0, -1), (1, 0), (-1, 0)]:
                            nx, ny = current_coord[0] + dx, current_coord[1] + dy
                            if 0 <= nx < max_x and 0 <= ny < max_y and maze[nx, ny] == 0:
                                new_node = Node((nx, ny), None, current_cost + 1)
                                if not tree.is_a_predecessor(current_node, new_node):
                                    current_node.add_child(new_node)  
                    tree.update_leaves()


        for i in to_be_routed:
            cleared_path, end = remove_tracks(maze, paths[i], rate)
            re_start = cleared_path[-1]
            start_node = Node(re_start, None, 0)
            tree = Tree(start_node)
            
            single_net_success, max_length = reroute_single_net(tree, max_length)
            if not single_net_success:
                return False, max_length
            else:
                continue

        return True, max_length

    max_x, max_y = maze.shape
    paths = [[] for _ in range(len(nets))]
    for i, net in enumerate(nets):
        try:
            start, end = net
        except ValueError:
            logger.exception(f"net should be a tuple (start, end), but the current net is {net}")
            raise ValueError(f"net should be a tuple (start, end), but the current net is {net}")
        else:
            maze[start] = 1
            maze[end] = 1
    for i, net in enumerate(nets):
        start, end = net
        maze[start] = 0
        maze[end] = 0
        path, maze = maze_routing(maze, nets, start, end, logger, experiment_dir, i)
        paths[i] = [start] + path

    # adjust length
    max_length = max(len(path) for path in paths)
    while True:
        to_be_routed = [i for i, path in enumerate(paths) if len(path) < max_length]
        success, max_length = reroute(to_be_routed, max_length, paths)
        if not success:
            continue
        else:
            break
    
    return paths, maze
        

                
