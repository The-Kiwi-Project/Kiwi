import heapq
import numpy as np 
import heapq
from routing_tree import *
from maze import Maze, VisualMaze


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


def maze_routing(maze, start, end, logger, color_mapper = 0, method = "maze"):
    '''
    routing a net from start to end

    maze: a 2D numpy array, 0 for free space, 1 for obstacle
    start: a tuple, (x, y)
    end: a tuple, (x, y)
    return: routed path & maze
    '''
    max_x, max_y = maze.shape()
    visited = np.full((max_x, max_y), " ")
    visited[start] = "B"
    maze.set_net_point(start, color_mapper)
    queue = [(0, start)]

    while queue:
        cost, (x, y) = heapq.heappop(queue)
        
        # find the end point
        if (x, y) == end:
            path = backtrace(visited, end)
            for p in path:
                maze.set_net_point(p, color_mapper)
            return path, maze
        
        # expand
        neighbours = [[(-1, 0), "S"], [(1, 0), "N"], [(0, -1), "E"], [(0, 1), "W"]]
        for (dx, dy), direc in neighbours:
            nx, ny = x + dx, y + dy
            if 0 <= nx < max_x and 0 <= ny < max_y and maze.value((nx, ny)) == 0 and visited[nx, ny] == " ":
                visited[nx, ny] = direc
                if method == "maze":
                    heapq.heappush(queue, (cost + 1, (nx, ny)))
                elif method == "A*":
                    heapq.heappush(queue, (cost + 1 + maze.Manhattan_distance((nx, ny), end)))
                else:
                    logger.error(f"maze_routing(): Invalid method {method}")
    
    logger.error(f"maze_routing(): Routing failed")
    exit()


def remove_tracks(maze, path, rate):
    l = len(path)
    cut_length = int(l * rate) if int(l * rate) > 0 else 1
    for coord in path[cut_length:]:
        maze.set_accessible(coord)

    return path[:cut_length], path[-1]


def bus_routing(maze, nets, logger, experiment_dir, rate = 0.8, show_expand = False, method="maze"):
    '''
    routing all nets in the maze

    maze: a 2D numpy array, 0 for free space, 1 for obstacle
    nets: a list of tuples, each tuple is (start, end)
    return: routed path & maze
    '''

    def reroute(to_be_routed, max_length, paths):
        def reroute_single_net(tree, max_length, vis_maze, net_i):
            while True:
                # the outermost nodes (to expand)
                wavefronts = tree.leaves

                if not wavefronts:
                    logger.error(f"routing failed for net {i}")
                    raise ValueError(f"routing failed for net {i}")
                else:
                    for current_node in wavefronts:
                        # a coordinate object
                        current_coord = current_node.coord

                        if current_coord == end:
                            re_routed_path = tree.backtrace(current_node)

                            # count the times of backtracing
                            maze.backtrace_count += 1
                            if show_expand:
                                vis_maze.backtrace_count += 1

                            # increase the max_length
                            if len(re_routed_path) + len(cleared_path) > max_length:
                                paths[i] = cleared_path + re_routed_path
                                max_length = len(paths[i])
                                for p in paths[i]:
                                    maze.set_net_point(p, i)
                                    if show_expand:
                                        vis_maze.set_special(p, 100)
                                if show_expand:
                                    vis_maze.show_matrix_expand(experiment_dir, net_i, save=True)
                                    vis_maze.show_search_counts(experiment_dir, save=True)
                                return False, max_length 
                            
                            # keep the max_length unchanged 
                            elif len(re_routed_path) + len(cleared_path) == max_length:
                                paths[i] = cleared_path + re_routed_path
                                for p in paths[i]:
                                    maze.set_net_point(p, i)
                                    if show_expand:
                                        vis_maze.set_special(p, 100)
                                if show_expand:
                                    vis_maze.show_matrix_expand(experiment_dir, net_i, save=True)
                                    vis_maze.show_search_counts(experiment_dir, save=True)
                                return True, max_length      
                            # still less than max_length     
                            else:
                                tree.remove_leaf(current_node)
                                continue
                        
                        # expand
                        for dx, dy in [(0, 1), (0, -1), (1, 0), (-1, 0)]:
                            nx, ny = current_coord[0] + dx, current_coord[1] + dy
                            if 0 <= nx < max_x and 0 <= ny < max_y and maze.value((nx, ny)) == maze.accessible:
                                new_node = Node((nx, ny), None, current_node.cost + 1)
                                if not tree.is_a_predecessor(current_node, new_node):
                                    current_node.add_child(new_node)  

                    # update outermost nodes, preparing for the next expand step               
                    updated_points = tree.update_leaves()
                    maze.search_count += len(updated_points)
                    if show_expand:
                        for p in updated_points:
                            vis_maze.matrix()[p.coord] -= 1
                        # vis_maze.show_matrix_expand(experiment_dir, net_i)
                        vis_maze.search_count_per_loop.append(len(updated_points))

                        
        for i in to_be_routed:
            # remove tracks
            cleared_path, end = remove_tracks(maze, paths[i], rate)
            re_start = cleared_path[-1]
            start_node = Node(re_start, None, 0)
            tree = Tree(start_node)

            # re_route
            if show_expand:
                vis_maze = VisualMaze(max_x)
                vis_maze.set_special(re_start, 10)
                vis_maze.set_special(end, 10)
                single_net_success, max_length = reroute_single_net(tree, max_length, vis_maze, i)
            else:
                single_net_success, max_length = reroute_single_net(tree, max_length, None, i)
            if not single_net_success:
                return False, max_length
            else:
                continue

        return True, max_length

    max_x, max_y = maze.shape()
    paths = [[] for _ in range(len(nets))]
    
    # initialize nets states and launch first round of routing
    if method == "maze":
        for i, net in enumerate(nets):
            try:
                start, end = net
            except ValueError:
                logger.exception(f"reroute(): net should be a tuple (start, end), but the current net is {net}")
            else:
                maze.set_obstacle(start)
                maze.set_obstacle(end)
        
        for i, net in enumerate(nets):
            start, end = net
            maze.set_accessible(start)
            maze.set_accessible(end)
            path, maze = maze_routing(maze, start, end, logger, i, method="maze")
            paths[i] = [start] + path
    elif method == "pathfinder":
        logger.error("reroute(): not implemented yet")
    else:
        message = f"reroute(): method should be either maze or pathfinder, but current method is {method}"
        logger.exception(message)

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
        

def segmentation_bus_routing_1(maze, nets, logger, experiment_dir, rate = 0.8, show_expand = False, method="maze"):
    # not implemented !!!

    def reroute(to_be_routed, max_length, paths):
        def reroute_single_net(start_node, max_length, vis_maze, net_i):
            assert start_node.coord != end, "start and end node is the same"

            tree = Tree(start_node)
            queue = [start_node]
            heapq.heapify(queue)

            while queue:
                current_node = heapq.heappop(queue)
                current_coord = current_node.coord

                if current_coord == end:
                    re_routed_path = tree.backtrace(current_node)

                    # count the times of backtracing
                    maze.backtrace_count += 1
                    if show_expand:
                        vis_maze.backtrace_count += 1

                    # increase the max_length
                    if len(re_routed_path) + len(cleared_path) > max_length:
                        paths[i] = cleared_path + re_routed_path
                        max_length = len(paths[i])
                        for p in paths[i]:
                            maze.set_net_point(p, i)
                            if show_expand:
                                vis_maze.set_special(p, 100)
                        if show_expand:
                            vis_maze.show_matrix_expand(experiment_dir, net_i, save=True)
                        return False, max_length 
                    
                    # keep the max_length unchanged 
                    elif len(re_routed_path) + len(cleared_path) == max_length:
                        paths[i] = cleared_path + re_routed_path
                        for p in paths[i]:
                            maze.set_net_point(p, i)
                            if show_expand:
                                vis_maze.set_special(p, 100)
                        if show_expand:
                            vis_maze.show_matrix_expand(experiment_dir, net_i, save=True)
                        return True, max_length      
                    # still less than max_length     
                    else:
                        parent_node = current_node.parent()
                        parent_node.remove_child(current_node)
                        continue
                
                # expand
                for dx, dy in [(0, 1), (0, -1), (1, 0), (-1, 0)]:
                    nx, ny = current_coord[0] + dx, current_coord[1] + dy
                    if 0 <= nx < max_x and 0 <= ny < max_y and maze.value((nx, ny)) == maze.accessible:
                        new_node = Node((nx, ny), None, current_node.cost + 1 + maze.Manhattan_distance((nx, ny), end))
                        if not tree.is_a_predecessor(current_node, new_node):
                            current_node.add_child(new_node) # 这一步就已经把new_node加到tree里面了
                            heapq.heappush(queue, new_node) 
                            maze.search_count += 1
                            if show_expand:
                                vis_maze.matrix()[new_node.coord] -= 1

            logger.exception("no path found")
            return False, max_length
                        
        for i in to_be_routed:
            # remove tracks
            cleared_path, end = remove_tracks(maze, paths[i], rate)
            re_start = cleared_path[-1]
            start_node = Node(re_start, None, 0)

            # re_route
            if show_expand:
                vis_maze = VisualMaze(max_x)
                vis_maze.set_special(re_start, 10)
                vis_maze.set_special(end, 10)
                single_net_success, max_length = reroute_single_net(start_node, max_length, vis_maze, i)
            else:
                single_net_success, max_length = reroute_single_net(start_node, max_length, None, i)
            if not single_net_success:
                return False, max_length
            else:
                continue

        return True, max_length

    max_x, max_y = maze.shape()
    paths = [[] for _ in range(len(nets))]
    
    # initialize nets states and launch first round of routing
    if method == "maze":
        for i, net in enumerate(nets):
            try:
                start, end = net
            except ValueError:
                logger.exception(f"reroute(): net should be a tuple (start, end), but the current net is {net}")
            else:
                maze.set_obstacle(start)
                maze.set_obstacle(end)
        
        for i, net in enumerate(nets):
            start, end = net
            maze.set_accessible(start)
            maze.set_accessible(end)
            path, maze = maze_routing(maze, start, end, logger, i, method="maze")
            paths[i] = [start] + path
    elif method == "pathfinder":
        logger.error("reroute(): not implemented yet")
    else:
        message = f"reroute(): method should be either maze or pathfinder, but current method is {method}"
        logger.exception(message)

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
   
