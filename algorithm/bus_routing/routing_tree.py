'''a routing tree, to describe the routing path of a net'''
import weakref


class Node:
    def __init__(self, coord, parent, cost):
        self.coord = coord
        self.parent = parent
        self.cost = cost
        self.children = []

    def add_child(self, child):
        '''child is a Node'''

        child.parent = weakref.ref(self)    # 避免循环引用
        self.children.append(child)

    def remove_child(self, child):
        assert child in self.children, f"{child.coord} is not a child of {self.coord}"
        child.parent = None
        self.children.remove(child)
    
    def get_parent(self):
        if self.parent:
            return self.parent()
        else:
            raise ValueError("This node has no parent")
    


class Tree:
    def __init__(self, root):
        self.root = root
        self.leaves = [self.root]
    
    def update_leaves(self) -> list:
        queue = [self.root]
        leaves = []
        while queue:
            node = queue.pop(0)
            if not node.children:
                leaves.append(node)
            else:
                queue.extend(node.children)
        self.leaves = leaves
        return leaves
    
    def is_a_predecessor(self, current_node, to_be_checked) -> bool:
        '''check if to_be_checked is one of the predecessors of current_node'''

        if current_node.coord == to_be_checked.coord:
            return True
        elif current_node.parent is None:
            return False
        else:
            return self.is_a_predecessor(current_node.parent(), to_be_checked)
    
    def backtrace(self, node) -> list:
        path = []
        while node.parent is not None:
            path.insert(0, node.coord) 
            node = node.parent()
        
        return path # not including the root

    def remove_leaf(self, node):
        assert node in self.leaves, f"{node.coord} is not a leaf of the tree"
        self.leaves.remove(node)
        node.get_parent().remove_child(node)
    
    
