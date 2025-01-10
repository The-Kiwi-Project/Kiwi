import sys
import os

source_path = './source'

def should_count(path: str) -> bool:
    return path.endswith('.h') or\
           path.endswith('.hh') or\
           path.endswith('.cc') or\
           path.endswith('.cpp') or\
           '.' not in path

def dir_sub_paths(dir_path):
    sub_paths = os.listdir(dir_path)
    sub_paths = filter(should_count, sub_paths)
    sub_paths = map(lambda path: f'{dir_path}/{path}', sub_paths)
    return sub_paths

def count_file_lines(file_path):
    with open(file_path, 'r') as file:
        line_count = sum([1 for _ in file])
    return line_count

def count_dir_lines(dir_path) -> int:
    sub_paths = dir_sub_paths(dir_path)

    def count_lines(path) -> int:
        return count_dir_lines(path) if os.path.isdir(path) else count_file_lines(path)
    
    return sum(map(count_lines, sub_paths))

print('Totol source file lines:', count_dir_lines(source_path))