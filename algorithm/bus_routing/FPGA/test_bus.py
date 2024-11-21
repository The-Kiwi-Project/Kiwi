from FPGA.bus_routing_FPGA import *
from FPGA.FPGA import FPGA, Coordinate, Direction
import random
import logging
import os
import time


def test_bus_routing_fpga(outputdir):
    try:
        experiment = "Experiment_1"
        experiment_dir = f"{outputdir}/{experiment}"
        rows = 9
        cols = 9
        nets = [(Coordinate(0, 0, Direction.LEFT, 0), Coordinate(4, 4, Direction.LEFT, 0)),
                (Coordinate(2, 2, Direction.LEFT, 0), Coordinate(5, 5, Direction.LEFT, 0)),]
        fpga = FPGA(rows, cols)

        os.makedirs(experiment_dir, exist_ok=True)
        logger = setup_logger(f"{experiment_dir}/bus_routing.log")
        fpga.init_logger(logger)
        logger.info(f"start bus routing experiment {experiment}")
        logger.info("initializing done")

        # route
        logger.info("start bus routing")
        start_time = time.time()
        paths, fpga = bus_routing(fpga, nets, logger, experiment_dir)
        end_time = time.time()
        logger.info(f"bus nets {[(net[0].coord, net[1].coord) for net in nets]} routing done successfully, on {rows}*{cols} fpga")
        logger.info(f"routing time: {end_time - start_time} seconds")

        # visualize result
        fpga.collect_results(nets, paths)
    
    except (ValueError) as e:
        logger.error(f"Error occurred in function test_bus_routing_fpga: {e}")


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
    outputdir = "./algorithm/bus_routing/fpga/expe_results"

    try:
        test_bus_routing_fpga(outputdir)

    except (FileNotFoundError, PermissionError) as e:
        print(f"Failed to open file in dir {outputdir}: {e}")
    





