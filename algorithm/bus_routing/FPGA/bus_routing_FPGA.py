import numpy as np 
from routing_tree import *
from FPGA.FPGA import FPGA, Coordinate, list_to_coord


def backtrace(visited, end: Coordinate) -> list[Coordinate]:
    path = []
    while visited[end.coord] != "B":
        path.insert(0, end)
        end = visited[end.coord]
    
    return path     


def maze_routing(
        fpga, 
        start, end,       # Coordinate 
        logger, experiment_dir, color_bias = 0):
    try:
        max_x, max_y = fpga.shape()

        visited = {}    # ((x, y), direction, index): "Pre_direction"
        visited[start.coord] = "B"
        fpga.set_value(start, color_bias)
        queue = [start]   #((x, y), direction, index)

        while queue:
            coord = queue.pop(0)
            
            if coord.coord == end.coord:
                path = backtrace(visited, end)
                for p in path:
                    fpga.set_value(p, color_bias)
                return path, fpga
            
            neighbours = fpga.expand(coord)
            for coordinate, predecessor in neighbours:
                if fpga.check_node_legality(coordinate) and coordinate.coord not in visited.keys():
                    visited[coordinate.coord] = predecessor
                    queue.append(coordinate)
        
        logger.info(f"Routing failed")
        print("Routing failed")
        exit()
    except Exception as e:
        logger.exception(f"Unexpected error occurred during maze_routing(): {e}")
        raise e

def remove_tracks(fpga, path, rate):
    '''return cleared path and the end point'''

    l = len(path)
    cut_length = int(l * rate) if int(l * rate) > 0 else 1
    for coord in path[cut_length:]:
        fpga.set_accessible(coord)

    return path[:cut_length], path[-1]


def bus_routing(fpga, 
                nets: list[tuple[Coordinate, Coordinate]], 
                logger, experiment_dir, rate = 0.8):

    try:
        def reroute(to_be_routed, max_length, paths):
            def reroute_single_net(tree, max_length, end: Coordinate):
                while True:
                    wavefronts = tree.leaves
                    if not wavefronts:
                        logger.info(f"routing failed for net {i}")
                        raise ValueError(f"routing failed for net {i}")
                    else:
                        for current_node in wavefronts:
                            current_cost = current_node.cost

                            if current_node.coord == end.coord:
                                re_routed_path = list_to_coord(tree.backtrace(current_node))
                                if len(re_routed_path) + len(cleared_path) > max_length:
                                    paths[i] = cleared_path + re_routed_path
                                    max_length = len(paths[i])
                                    for p in paths[i]:
                                        fpga.set_value(p, i)
                                    return False, max_length # 最大长度有变
                                elif len(re_routed_path) + len(cleared_path) == max_length:
                                    paths[i] = cleared_path + re_routed_path
                                    for p in paths[i]:
                                        fpga.set_value(p, i)
                                    return True, max_length # 最大长度没变           
                                else:
                                    tree.remove_leaf(current_node)
                                    continue
                            
                            current_coordinate = Coordinate(
                                                    current_node.coord[0], current_node.coord[1], current_node.direction, current_node.index
                                                )
                            neighbours = fpga.expand(current_coordinate)
                            for coordinate, _ in neighbours:
                                if fpga.check_node_legality(coordinate):
                                    new_node = Node(coordinate.coord, current_node, current_cost + 1)
                                    if not tree.is_a_predecessor(current_node, new_node):
                                        current_node.add_child(new_node)  
                        tree.update_leaves()
                            
            for i in to_be_routed:
                cleared_path, end = remove_tracks(maze, paths[i], rate)
                re_start = cleared_path[-1]
                start_coord = Node(re_start.coord, None, 0)    # 这里的Node里面的coord是一个tuple
                tree = Tree(start_coord)

                single_net_success, max_length = reroute_single_net(tree, max_length, end)
                if not single_net_success:
                    return False, max_length
                else:
                    continue

            return True, max_length

        paths = [[] for _ in range(len(nets))]
        
        # initialize nets states and first round of routing
        for i, net in enumerate(nets):
            try:
                start, end = net
            except ValueError:
                logger.exception(f"net should be a tuple (start, end), but the current net is {net}")
                raise ValueError(f"net should be a tuple (start, end), but the current net is {net}")
            else:
                fpga.set_obstacle(start)
                fpga.set_obstacle(end)
        
        for i, net in enumerate(nets):
            start, end = net
            fpga.set_accessible(start)
            fpga.set_accessible(end)
            path, maze = maze_routing(fpga, start, end, logger, experiment_dir, i)
            paths[i] = [start] + path   # Coordinate

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
    except Exception as e:
        logger.exception(f"An error occurred during bus_routing: {e}")
        raise e
        

                
