from bus_routing_asic import *
import logging
import os
import time


def test_bus_routing_asic(outputdir):
    try:
        experiment = "Experiment_6"
        experiment_dir = f"{outputdir}/{experiment}"
        size = 100
        nets = [
            [(1, 5), (9, 20)],
            [(10, 8), (80, 35)]
        ]
        maze = Maze(size)

        os.makedirs(experiment_dir, exist_ok=True)
        logger = setup_logger(f"{experiment_dir}/bus_routing.log")
        maze.init_logger(logger)
        logger.info(f"start bus routing experiment {experiment}")
        logger.info("initializing done")
        maze.show_nets("initial state", nets, experiment_dir, save=True)

        # route
        logger.info("start bus routing")
        start_time = time.time()
        paths, maze = segmentation_bus_routing_1(maze, nets, logger, experiment_dir, show_expand=False)
        end_time = time.time()
        logger.info(f"bus nets {nets} routing done successfully, on {size}*{size} maze")
        logger.info(f"routing time: {end_time - start_time} seconds")

        # visualize result
        maze.show_nets("Routing Done", nets, experiment_dir, save=True)
        maze.collect_results(nets)
    
    except (ValueError) as e:
        logger.error(f"Error occurred in function test_bus_routing_fpga: {e}")


def setup_logger(logfile):
    logger = logging.getLogger('BusRoutingLogger')
    logger.setLevel(logging.DEBUG)  # 设置日志级别

    file_handler = logging.FileHandler(logfile, mode='a')
    file_handler.setLevel(logging.DEBUG)  # 文件日志的级别

    formatter = logging.Formatter('%(asctime)s - %(levelname)s - %(message)s')
    file_handler.setFormatter(formatter)

    logger.addHandler(file_handler)
    
    return logger



if __name__ == '__main__':
    outputdir = "./algorithm/bus_routing/asic/expe_results"

    try:
        test_bus_routing_asic(outputdir)

    except (FileNotFoundError, PermissionError) as e:
        print(f"Failed to open file in dir {outputdir}: {e}")
    





