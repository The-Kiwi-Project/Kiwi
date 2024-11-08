from bus_routing_by_xl import *
import random
import logging
import os


def test_bus_routing(outputdir):
    try:
        # initialize
        experiment = "Experiment_2"
        experiment_dir = f"{outputdir}/{experiment}"
        maze_size = 50
        nets = [[(1, 5), (10, 9)], [(2, 7), (40, 25)], [(3, 8), (20, 30)], [(4, 6), (30, 40)]]
        maze = np.zeros((maze_size, maze_size))

        os.makedirs(experiment_dir, exist_ok=True)
        logger = setup_logger(f"{experiment_dir}/bus_routing.log")
        logger.info(f"start bus routing experiment {experiment}")
        logger.info("initializing done")
        visualize(maze, "initial maze", nets, experiment_dir)

        # route
        logger.info("start bus routing")
        paths, maze = bus_routing(maze, nets, logger, experiment_dir)
        logger.info(f"bus nets {nets} routing done successfully, on {maze_size}*{maze_size} maze")

        # visualize result
        visualize(maze, "final maze", nets, experiment_dir)
        collect_result(maze, nets, logger)
    
    except (ValueError) as e:
        logger.error(f"Error occurred in function bus_routing: {e}")
    
    


def setup_logger(logfile):
    # 配置日志记录器
    logger = logging.getLogger('BusRoutingLogger')
    logger.setLevel(logging.DEBUG)  # 设置日志级别

    # 创建文件处理器，将日志写入文件
    file_handler = logging.FileHandler(logfile, mode='a')
    file_handler.setLevel(logging.DEBUG)  # 文件日志的级别

    # 创建日志格式器，并设置日志格式
    formatter = logging.Formatter('%(asctime)s - %(levelname)s - %(message)s')
    file_handler.setFormatter(formatter)

    # 将处理器添加到日志记录器
    logger.addHandler(file_handler)
    
    return logger



if __name__ == '__main__':
    outputdir = "./algorithm/bus_routing_by_xl/expe_results"

    try:
        test_bus_routing(outputdir)

    except (FileNotFoundError, PermissionError) as e:
        print(f"Failed to open file in dir {outputdir}: {e}")
    





