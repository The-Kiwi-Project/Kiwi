from bus_routing_by_xl import backtrace, maze_routing
import numpy as np
import unittest


class TestMazeRouting(unittest.TestCase):

    def test_maze_routing_simple(self):
        maze = np.array([
            [0, 0, 0, 0],
            [0, 1, 1, 1],
            [0, 1, 1, 0],
            [0, 0, 0, 0]
        ])
        start = (0, 0)
        end = (3, 3)
        expected_path = [(1, 0), (2, 0), (3, 0), (3, 1), (3, 2), (3, 3)]
        expected_maze = np.array([
            [2, 0, 0, 0],
            [2, 1, 1, 1],
            [2, 1, 1, 0],
            [2, 2, 2, 2]
        ])
        path, maze = maze_routing(maze, start, end)
        self.assertEqual(path, expected_path)
        np.testing.assert_array_equal(maze, expected_maze)

    def test_maze_routing_no_path(self):
        maze = np.array([
            [0, 0, 0, 0],
            [1, 1, 1, 1],
            [0, 1, 1, 0],
            [0, 0, 0, 0]
        ])
        start = (0, 0)
        end = (3, 2)
        with self.assertRaises(SystemExit):
            maze_routing(maze, start, end)

    def test_maze_routing_start_on_end(self):
        maze = np.array([
            [0, 0, 0, 0],
            [0, 1, 1, 0],
            [0, 1, 1, 0],
            [0, 0, 0, 0]
        ])
        start = (0, 0)
        end = (0, 0)
        expected_path = []
        expected_maze = np.array([
            [2, 0, 0, 0],
            [0, 1, 1, 0],
            [0, 1, 1, 0],
            [0, 0, 0, 0]
        ])
        path, maze = maze_routing(maze, start, end)
        self.assertEqual(path, expected_path)
        np.testing.assert_array_equal(maze, expected_maze)



if __name__ == "__main__":
    unittest.main()
