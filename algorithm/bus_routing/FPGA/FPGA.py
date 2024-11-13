from enum import Enum




class Direction(Enum):
    UP = 0
    DOWN = 1
    LEFT = 2
    RIGHT = 3


class Coordinate:
    '''x -> row, y -> column'''
    def __init__(self, x, y, direction, index) -> None:
        self.coord = ((x, y), direction, index)
    

def list_to_coord(list: list[tuple]) -> list[Coordinate]:
    coords = []
    for c in list:
        posi, direction, index = c
        coords.append(Coordinate(posi[0], posi[1], direction, index))
    return coords


class CobUnit:
    '''只负责变换，不检查xy坐标'''
    def __init__(self) -> None:
        self.size = 8
        self.logger = None
    
    def init_logger(self, logger):
        self.logger = logger
    
    def handle_error(self, message, error_obj):
        if self.logger:
            self.logger.error(message, error_obj)
        raise error_obj(message)

    def direction_transform(self, from_coord, to_dir) -> tuple:
        _, direction, _ = from_coord

        if direction == Direction.UP:
            if to_dir == Direction.LEFT:
                return self.up_to_left(from_coord)
            elif to_dir == Direction.RIGHT:
                return self.up_to_right(from_coord)
            elif to_dir == Direction.DOWN:
                return self.vertical(from_coord)
            else:
                self.handle_error(f"invalid direction, from {direction} to {to_dir}", ValueError)
        elif direction == Direction.DOWN:
            if to_dir == Direction.LEFT:
                return self.down_to_left(from_coord)
            elif to_dir == Direction.RIGHT:
                return self.down_to_right(from_coord)
            elif to_dir == Direction.UP:
                return self.vertical(from_coord)
            else:
                self.handle_error(f"invalid direction, from {direction} to {to_dir}", ValueError)
        elif direction == Direction.LEFT:
            if to_dir == Direction.UP:
                return self.left_to_up(from_coord)
            elif to_dir == Direction.DOWN:
                return self.left_to_down(from_coord)
            elif to_dir == Direction.RIGHT:
                return self.horizontal(from_coord)
            else:
                self.handle_error(f"invalid direction, from {direction} to {to_dir}", ValueError)
        elif direction == Direction.RIGHT:
            if to_dir == Direction.UP:
                return self.right_to_up(from_coord)
            elif to_dir == Direction.DOWN:
                return self.right_to_down(from_coord)
            elif to_dir == Direction.LEFT:
                return self.horizontal(from_coord)
            else:
                self.handle_error(f"invalid direction, from {direction} to {to_dir}", ValueError)
        else:
            self.handle_error(f"invalid direction, from {direction} to {to_dir}", ValueError)

    def norm_direc_trans(self, from_coord, to_dir) -> tuple:
        return (self.norm_coord(self.direction_transform(from_coord, to_dir))) 

    def norm_coord(self, coord) -> tuple:
        '''use normalize function to transform the coord to the form with direction = Down or Left'''
        posi, direc, index = coord
        if direc == Direction.UP:
            return ((posi[0]+1, posi[1]), Direction.DOWN, index)
        elif direc == Direction.RIGHT:
            return ((posi[0], posi[1]+1), Direction.LEFT, index)
        else:
            return coord
          
    def right_to_up(self, coord) -> tuple:
        posi, direction, index = coord
        assert direction == Direction.RIGHT, f"direction should be right, not {direction}"

        if index == 0:
            return ((posi[0], posi[1]), Direction.UP, 7)
        else:
            return ((posi[0], posi[1]), Direction.UP, index - 1)
    
    def right_to_down(self, coord) -> tuple:
        posi, direction, index = coord
        assert direction == Direction.RIGHT, f"direction should be right, not {direction}"

        if index == 7:
            return ((posi[0], posi[1]), Direction.DOWN, 7)
        else:
            return ((posi[0], posi[1]), Direction.DOWN, 6 - index)
    
    def up_to_right(self, coord) -> tuple:
        posi, direction, index = coord
        assert direction == Direction.UP, f"direction should be up, not {direction}"

        if index == 7:
            return ((posi[0], posi[1]), Direction.RIGHT, 0)
        else:
            return ((posi[0], posi[1]), Direction.RIGHT, index + 1)
    
    def up_to_left(self, coord) -> tuple:
        posi, direction, index = coord
        assert direction == Direction.UP, f"direction should be up, not {direction}"

        if index == 7:
            return ((posi[0], posi[1]), Direction.LEFT, 7)
        else:
            return ((posi[0], posi[1]), Direction.LEFT, 6 - index)
    
    def down_to_left(self, coord) -> tuple:
        posi, direction, index = coord
        assert direction == Direction.DOWN, f"direction should be down, not {direction}"

        if index == 0:
            return ((posi[0], posi[1]), Direction.LEFT, 7)
        else:
            return ((posi[0], posi[1]), Direction.LEFT, index - 1)
    
    def down_to_right(self, coord) -> tuple:
        posi, direction, index = coord
        assert direction == Direction.DOWN, f"direction should be down, not {direction}"

        if index == 7:
            return ((posi[0], posi[1]), Direction.RIGHT, 7)
        else:
            return ((posi[0], posi[1]), Direction.RIGHT, 6 - index)
    
    def left_to_down(self, coord) -> tuple:
        posi, direction, index = coord
        assert direction == Direction.LEFT, f"direction should be left, not {direction}"

        if index == 7:
            return ((posi[0], posi[1]), Direction.DOWN, 0)
        else:
            return ((posi[0], posi[1]), Direction.DOWN, index + 1)
    
    def left_to_up(self, coord) -> tuple:
        posi, direction, index = coord
        assert direction == Direction.LEFT, f"direction should be left, not {direction}"

        if index == 7:
            return ((posi[0], posi[1]), Direction.UP, 7)
        else:
            return ((posi[0], posi[1]), Direction.UP, 6 - index)
    
    def horizontal(self, coord) -> tuple:
        posi, direction, index = coord
        if direction == Direction.LEFT:
            return ((posi[0], posi[1]), Direction.RIGHT, index)
        elif direction == Direction.RIGHT:
            return ((posi[0], posi[1]), Direction.LEFT, index)
        else:
            self.handle_error(f"illegal direction {direction}, should be Left or Right", ValueError)

    def vertical(self, coord) -> tuple:
        posi, direction, index = coord
        if direction == Direction.UP:
            return ((posi[0], posi[1]), Direction.DOWN, index)
        elif direction == Direction.DOWN:
            return ((posi[0], posi[1]), Direction.UP, index)
        else:
            self.handle_error(f"illegal direction {direction}, should be Up or Down", ValueError)


class Cob:
    """make sure the input (x, y) coordinate is legal"""

    def __init__(self):
        self.size = 128
        self.cobunit_num = 16
        self.cobunit_size = 8
        self.cobunit = CobUnit()
        self.logger = None

    def init_logger(self, logger):
        self.logger = logger
        self.cobunit.init_logger(logger)

    def handle_error(self, message, error_obj):
        if self.logger is not None:
            self.logger.error(message)
        raise error_obj(message)

    def neighbours(self, track_coord) -> list[tuple]:
        posi, direction, index = track_coord

        if index < 0 or index >= self.size:
            self.handle_error(f"index {index} out of range", ValueError)

        cobunit, cobunit_coord = self.cob_to_cobunit(self.track_to_cob(track_coord))
        neighbours_list = []  # (((x, y), direction, index), predecessor)

        if direction == Direction.UP:
            neighbours_list.extend([
                (self.cobunit.norm_direc_trans(cobunit_coord, Direction.DOWN), 
                 self.cobunit.norm_coord(track_coord)),
                (self.cobunit.norm_direc_trans(cobunit_coord, Direction.LEFT), 
                 self.cobunit.norm_coord(track_coord)),
                (self.cobunit.norm_direc_trans(cobunit_coord, Direction.RIGHT), 
                 self.cobunit.norm_coord(track_coord))
            ])
        elif direction == Direction.DOWN:
            neighbours_list.extend([
                (self.cobunit.norm_direc_trans(cobunit_coord, Direction.UP), 
                 self.cobunit.norm_coord(track_coord)),
                (self.cobunit.norm_direc_trans(cobunit_coord, Direction.LEFT), 
                 self.cobunit.norm_coord(track_coord)),
                (self.cobunit.norm_direc_trans(cobunit_coord, Direction.RIGHT), 
                 self.cobunit.norm_coord(track_coord))
            ])
        elif direction == Direction.LEFT:
            neighbours_list.extend([
                (self.cobunit.norm_direc_trans(cobunit_coord, Direction.UP), 
                 self.cobunit.norm_coord(track_coord)),
                (self.cobunit.norm_direc_trans(cobunit_coord, Direction.DOWN), 
                 self.cobunit.norm_coord(track_coord)),
                (self.cobunit.norm_direc_trans(cobunit_coord, Direction.RIGHT), 
                 self.cobunit.norm_coord(track_coord))
            ])
        elif direction == Direction.RIGHT:
            neighbours_list.extend([
                (self.cobunit.norm_direc_trans(cobunit_coord, Direction.UP), 
                 self.cobunit.norm_coord(track_coord)),
                (self.cobunit.norm_direc_trans(cobunit_coord, Direction.DOWN), 
                 self.cobunit.norm_coord(track_coord)),
                (self.cobunit.norm_direc_trans(cobunit_coord, Direction.LEFT), 
                 self.cobunit.norm_coord(track_coord))
            ])

        nei_tracks = [
            (self.cob_to_track(self.cobunit_to_cob(nei_coord, cobunit)), predecessor)
            for nei_coord, predecessor in neighbours_list
        ]
        return nei_tracks

    def track_to_cob(self, track_coord) -> tuple:
        posi, direction, index = track_coord

        if index < 0 or index >= self.size:
            self.handle_error(f"index {index} out of range", ValueError)

        bank = index // 64
        cobunit = index % 8
        cobunit_index = (index - bank * 64) // 8
        return ((posi[0], posi[1]), direction, 64 * bank + 8 * cobunit + cobunit_index)

    def cob_to_track(self, cob_coord) -> tuple:
        posi, direction, index = cob_coord

        if index < 0 or index >= self.size:
            self.handle_error(f"index {index} out of range", ValueError)

        bank = index // 64
        cobunit = (index - 64 * bank) // 8
        cobunit_index = index % 8
        return ((posi[0], posi[1]), direction, 64 * bank + 8 * cobunit + cobunit_index)

    def cob_to_cobunit(self, cob_coord) -> tuple:
        posi, direction, index = cob_coord

        if index < 0 or index >= self.size:
            self.handle_error(f"index {index} out of range", ValueError)

        cobunit = index // 8
        cobunit_index = index % 8
        return cobunit, ((posi[0], posi[1]), direction, cobunit_index)

    def cobunit_to_cob(self, cobunit_coord, cobunit) -> tuple:
        posi, direc, index = cobunit_coord
        return (posi, direc, cobunit * 8 + index)


class FPGA:
    """Interface with external operations, using Coordinate for the interface;
    uses tuples for cob internally. Ensures coordinates passed to hardware are valid."""

    def __init__(self, rows, cols):
        self.rows = rows
        self.cols = cols
        self.cob = Cob()
        self.logger = None
        self.accessible = 0
        self.obstacle = 1
        self.net = 2
        self.color_mapper = {}
        self.plot_counter = 0

        for i in range(rows):
            for j in range(cols):
                for index in range(128):
                    for direction in (Direction.UP, Direction.DOWN, Direction.LEFT, Direction.RIGHT):
                        self.color_mapper[((i, j), direction, index)] = self.accessible
        for j in range(cols):
            for index in range(128):
                self.color_mapper[((rows, j), Direction.DOWN, index)] = self.accessible
        for i in range(rows):
            for index in range(128):
                self.color_mapper[((i, cols), Direction.LEFT, index)] = self.accessible

    def init_logger(self, logger):
        self.logger = logger
        self.cob.init_logger(logger)

    def handle_error(self, message, error_obj):
        if self.logger:
            self.logger.error(message)
        raise error_obj(message)

    def shape(self):
        return self.rows, self.cols
    
    def check_node_within_range(self, coord:Coordinate) -> bool:
        posi, direction, _ = coord.coord

        if 0 <= posi[0] < self.rows and 0 <= posi[1] < self.cols:
            is_valid = True
        elif direction == Direction.DOWN and posi[0] == self.rows and 0 <= posi[1] < self.cols:
            is_valid = True
        elif direction == Direction.LEFT and 0 <= posi[0] < self.rows and posi[1] == self.cols:
            is_valid = True
        else:
            is_valid = False
        return is_valid

    def check_node_legality(self, coord: Coordinate) -> bool:
        if self.check_node_within_range(coord):    
            try:
                return self.color_mapper[coord.coord] == self.accessible
            except (KeyError) as e: 
                self.handle_error(f"check_node_legality(): self.color_mapper should has the key {coord.coord}, but not", e)
        
        return False

    def set_value(self, coord: Coordinate, color_bias: int):
        if not self.check_node_within_range(coord):
            self.handle_error(f"set_value(): coord {coord.coord} out of range", ValueError)

        self.color_mapper[coord.coord] = self.net + color_bias

    def expand(self, coord: Coordinate) -> list[tuple]:
        if not self.check_node_within_range(coord):
            self.handle_error(f"expand(): coord {coord.coord} out of range", ValueError)

        posi, direction, index = coord.coord
        neighbour_coords = []
        if direction == Direction.UP:
            neighbours = self.cob.neighbours(coord.coord) + self.cob.neighbours(((posi[0] + 1, posi[1]), Direction.DOWN, index))
        elif direction == Direction.DOWN:
            neighbours = self.cob.neighbours(coord.coord) + self.cob.neighbours(((posi[0] - 1, posi[1]), Direction.UP, index))
        elif direction == Direction.LEFT:
            neighbours = self.cob.neighbours(coord.coord) + self.cob.neighbours(((posi[0], posi[1] - 1), Direction.RIGHT, index))
        elif direction == Direction.RIGHT:
            neighbours = self.cob.neighbours(coord.coord) + self.cob.neighbours(((posi[0], posi[1] + 1), Direction.LEFT, index))
        else:
            self.handle_error(f"expand(): direction {direction} not supported", ValueError)

        for post, pre in neighbours:
            posi_post, direc_post, index_post = post
            posi_pre, direc_pre, index_pre = pre
            neighbour_coords.append((
                Coordinate(posi_post[0], posi_post[1], direc_post, index_post),
                Coordinate(posi_pre[0], posi_pre[1], direc_pre, index_pre)
            ))
        return neighbour_coords

    def set_accessible(self, coord: Coordinate):
        if not self.check_node_within_range(coord):
            self.handle_error(f"set_accessible(): coord {coord.coord} out of range", ValueError)

        self.color_mapper[coord.coord] = self.accessible

    def set_obstacle(self, coord: Coordinate):
        if not self.check_node_within_range(coord):
            self.handle_error(f"set_obstacle(): coord {coord.coord} out of range", ValueError)

        self.color_mapper[coord.coord] = self.obstacle

    def re_init(self):
        self.color_mapper = {}

    def set_special(self, coord: Coordinate, value):
        if not self.check_node_within_range(coord):
            self.handle_error(f"set_special(): coord {coord.coord} out of range", ValueError)

        self.color_mapper[coord.coord] = value
    
    def collect_results(self, nets, paths):
        if self.logger:
            self.logger.info("**********************Routing Results***********************")
            for i in range(len(nets)):
                counter = 0
                for value in self.color_mapper.values():
                    if value == i + 2:
                        counter += 1
                self.logger.info(f"Net {i} has {counter} tracks")
            
            for i, path in enumerate(paths):
                self.logger.info(f"Net {i}: ")
                for p in path:
                    self.logger.info(p.coord)
            self.logger.info("**************************************************************")
        else:
            raise ValueError("No logger provided")


    