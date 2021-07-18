import sys
import ast

#O(1) average time complexity.
class ListNode:
    def __init__(self, key, value):
        self.prev = None
        self.next = None
        self.key = key
        self.value = value

class DoubleLinkedList:
    def __init__(self):
        self.head = None
        self.tail = None
        self.count = 0
    
    def push_front(self, node):
        if self.head:
            self.head.prev = node
            node.next = self.head

        self.head = node

        if not self.tail:
            self.tail = node
        self.count += 1

    def pop_front(self):
        result = self.head
        if self.head:
            self.head = self.head.next

        if self.head:
            self.head.prev = None
        else:
            self.tail = None
        self.count -= 1
        return result

    def push_back(self, node):
        if self.tail:
            self.tail.next = node
            node.prev = self.tail
        
        if not self.head:
            self.head = node
        self.tail = node
        self.count += 1
    
    def pop_back(self):
        result = self.tail
        if self.tail:
            self.tail = self.tail.prev
        if self.tail:
            self.tail.next = None
        else:
            self.head = None
        self.count -= 1
        return result
    
    def remove_node(self, node):
        if node.next and node.prev:
            node.prev.next, node.next.prev = node.next, node.prev
            node.next, node.prev = None, None   # removed node point to null
        elif node.prev:
            self.pop_back()
        else:
            self.pop_front()
        self.count -= 1
    
    def insert_node(self, prev, node):
        next = prev.next
        node.next = prev.next
        node.prev = prev
        prev.next = node
        if next:
            next.prev = node
        self.count += 1

    def dump(self):
        result = []
        curr = self.head
        while curr:
            result.append(curr.key)
            curr = curr.next
        print(result)

class LRUCache:

    def __init__(self, capacity: int):
        self.capacity = capacity
        self.dict = {}
        self.cache = DoubleLinkedList()

    def get(self, key: int) -> int:
        result = -1
        if key in self.dict:
            self.cache.remove_node(self.dict[key])
            result = self.dict[key].value
            self.cache.push_front(self.dict[key])
        return result

    def put(self, key: int, value: int) -> None:
        if key in self.dict:
            self.cache.remove_node(self.dict[key])
            self.dict[key].value = value
            self.cache.push_front(self.dict[key])

        elif self.cache.count < self.capacity:
            node = ListNode(key, value)
            self.dict[key] = node
            self.cache.push_front(node)

        else:
            node = ListNode(key, value)
            self.dict[key] = node

            tmp = self.cache.pop_back()
            del self.dict[tmp.key]
            self.cache.push_front(node)

    def dump(self):
        self.cache.dump()


if __name__ == "__main__":
    #f = open('test/test3')
    #lines = f.read().split('\n')
    lines = []
    for line in sys.stdin:
        lines.append(line)
    methods = ast.literal_eval(lines[0])
    params = ast.literal_eval(lines[1])
    obj = None
    for i in range(len(methods)):
        if methods[i] == 'put':
            obj.put(params[i][0], params[i][1])
        elif methods[i] == 'get':
            obj.get(params[i][0])
        else:
            obj = LRUCache(params[i][0])

        obj.dump()
    print('Done')  

'''
# Your LRUCache object will be instantiated and called as such:
obj = LRUCache(2)
obj.put(1,1)
obj.put(2,2)
obj.get(1)
obj.put(3,3)
obj.get(2)
obj.put(4,4)
obj.get(1)
obj.get(3)
obj.get(4)
#obj.put(3,3)


obj1 = LRUCache(2)
obj1.put(2,1)
obj1.put(2,2)
obj1.get(2)
obj1.put(1,1)
obj1.put(4,1)
obj1.get(2)
'''