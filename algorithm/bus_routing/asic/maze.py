'''Class Maze for data collection and processing during the process of routing'''

import numpy as np
import matplotlib.pyplot as plt
from matplotlib.colors import ListedColormap
from matplotlib import cm


class Maze:
    def __init__(self, size):
        self.maze = np.zeros((size, size))
        self.accessible = 0
        self.obstacle = 1
        self.number_for_net = 2
        self.logger = None
        self.backtrace_count = 0        # 统计重布线的时候回溯了多少次
        self.search_count = 0           # 统计重布线的时候搜索了多少次

    def re_init(self):
        self.maze = np.zeros((self.maze.shape[0], self.maze.shape[1]))
        self.logger = None
        self.backtrace_count = 0
        self.search_count = 0

    def init_logger(self, logger):
        self.logger = logger
    
    def handle_exception(self, message, error_obj):
        if self.logger is not None:
            self.logger.exception(message)
        raise error_obj(message)
    
    def value(self, coord):
        '''return the value of the point in maze'''

        if 0 <= coord[0] < self.maze.shape[0] and 0 <= coord[1] < self.maze.shape[1]:
            return self.maze[coord]
        else:
            self.handle_exception(f'Invalid coordinate: {coord}', ValueError)
    
    def set_net_point(self, coord, color_mapper=None):
        '''set value of the point at <coord> in order to visualize'''

        if color_mapper is not None and color_mapper < 0:
            self.handle_exception(f'Invalid color mapper: {color_mapper}', ValueError)
        
        if 0 <= coord[0] < self.maze.shape[0] and 0 <= coord[1] < self.maze.shape[1]:
            self.maze[coord] = self.number_for_net if color_mapper is None else self.number_for_net + color_mapper
        else:
            self.handle_exception(f'Invalid coordinate: {coord}', ValueError)
    
    def set_obstacle(self, coord):
        '''set a point as an obstacle'''

        if 0 <= coord[0] < self.maze.shape[0] and 0 <= coord[1] < self.maze.shape[1]:
            self.maze[coord] = self.obstacle
        else:
            self.handle_exception(f'Invalid coordinate: {coord}', ValueError)

    def set_accessible(self, coord):
        '''set a point as accessible'''

        if 0 <= coord[0] < self.maze.shape[0] and 0 <= coord[1] < self.maze.shape[1]:
            self.maze[coord] = self.accessible
        else:
            self.handle_exception(f'Invalid coordinate: {coord}', ValueError)
    
    def set_special(self, coord, value):
        '''bind a point with a special value'''
        if 0 <= coord[0] < self.maze.shape[0] and 0 <= coord[1] < self.maze.shape[1]:
            self.maze[coord] = value
        else:
            self.handle_exception(f'Invalid coordinate: {coord}', ValueError)

    def shape(self):
        return self.maze.shape

    def matrix(self):
        return self.maze

    def Manhattan_distance(self, coord1, coord2):
        return abs(coord1[0]-coord2[0]) + abs(coord1[1]-coord2[1])

    def visualize(self, title):
        '''base visualizing function'''

        plt.clf()
        num_classes = int((self.matrix().max()-self.matrix().min())+1)

        if num_classes > 10:    
            cmap = cm.get_cmap('viridis')
        else:
            colors = cm.get_cmap("tab20", num_classes)
            cmap = ListedColormap([colors(i) for i in range(num_classes)])

        plt.imshow(self.matrix(), cmap=cmap)
        plt.grid(color='gray', linestyle='-', linewidth=0.5)

        mx, my = self.shape()
        plt.xticks(np.arange(-0.5, mx - 0.5, 1), [str(i) for i in range(mx)], fontsize=8)
        plt.yticks(np.arange(-0.5, my - 0.5, 1), [str(i) for i in range(my)], fontsize=8)

        plt.gca().xaxis.set_ticks_position('top')
        plt.gca().tick_params(axis='x', rotation=45)

        if num_classes > 10:  
            ticks = np.linspace(self.matrix().min(), self.matrix().max(), 10)
        else:
            ticks = [i for i in range(num_classes)]
        plt.colorbar(ticks=ticks) 
        plt.title(title)

    def show_nets(self, title, nets, savedir, save=False):
        '''visualize all nets'''

        self.visualize(title)

        for i, net in enumerate(nets):
            for point in net:
                plt.text(point[1], point[0], str(i), ha="center", va="center", color="black", fontsize=8-self.maze.shape[0]/30)

        if save:
            savepath = f"{savedir}/{title}"
            plt.savefig(savepath, dpi=300, bbox_inches='tight')

        plt.show()
    
    def collect_results(self, nets):
        '''call after successfully routed'''

        if self.logger:
            for i in range(len(nets)):
                count = np.count_nonzero(self.matrix() == (2+i))
                self.logger.info(f"net {i} has length={count}")
            self.logger.info(f"total backtrace count = {self.backtrace_count}")
            self.logger.info(f"total search count = {self.search_count}")
        else:
            raise Exception("No logger provided")

        
class VisualMaze(Maze):
    '''maze for showing expanding process'''

    def __init__(self, size):
        super().__init__(size)
        self.search_count_per_loop = []
    
    def show_matrix_expand(self, savedir, net_i, save=False):
        '''visualize the expanding process'''

        title = f"{savedir}/{str(net_i)+'_'+str(self.backtrace_count)}"
        self.visualize(title)

        if save:
            plt.savefig(title, dpi=300, bbox_inches='tight')
    
    def show_search_counts(self, savedir, save = False):
        '''visualize the search counts in each search'''

        plt.clf()
        plt.plot(self.search_count_per_loop)
        plt.xlabel("Loops")
        plt.ylabel("Search Count")
        plt.title(f"search_times_per_loop")
        if save:
            plt.savefig(f"{savedir}/search_times_per_loop", dpi=300, bbox_inches='tight')
        plt.show()


