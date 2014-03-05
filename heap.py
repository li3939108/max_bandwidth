import array
import random
class Heap:
	def __init__(self, 
		max_size, 
		key_value_pairs = None, 
		min_or_max = "max"):
		"""
		max_size is maximum heap size
		This heap implementation will store the key and value in two arrays, 
		and only the self.values[self.keys[index]] will give the value on index 
		and the key should be a subset of integers from 1 to max_size
		The default heap is max-heap, if you want a min-heap, simply make the third argument not "max"
		"""
		self.sorted = False
		self.max_size = max_size
		self.values = array.array('I', [0] * (max_size + 1))
		if(key_value_pairs == None):
			self.indices = array.array('I', range(0, max_size + 1))
			self.keys = array.array('I', range(0, max_size + 1))
			self.values = array.array('I', map(random.randrange, [1000] * (max_size + 1)))
			self.size = self.max_size
		else:
			self.indices = array.array('I', [0] * (max_size + 1))
			self.keys = array.array('I', [0] * (max_size + 1))
			self.size = len(key_value_pairs)
			for i in range(0, len(key_value_pairs)):
				self.keys[i + 1] = key_value_pairs[i][0]
				self.values[self.keys[i + 1]] = key_value_pairs[i][1]
				self.indices[self.keys[i + 1]] = i + 1
		self.set_type(min_or_max)
		self.build_heap()
	def set_type(self, min_or_max):
		if(min_or_max == "max"): 
			self.t = "max"
			self.heapify = self.max_heapify
			self.update = self.max_update
		else:
			self.t = "min"
			self.heapify = self.min_heapify
			self.update = self.min_update
	def __getitem__(self, key):
		return self.values[key]
	def __setitem__(self, key, value):
		if(self.indices[key] == 0):
			self.insert(key, value)
		else:
			self.update(key, value)
	def m(self):
		return [self.keys[1], self.values[self.keys[1]]]
	def parent(self, key):
		index = self.indices[key]
		return self.keys[parent(index)]
	def left(self, key):
		index = self.indices[key]
		return self.keys[left(index)]
	def right(self, key):
		index = self.indices[key]
		return self.keys[right(index)]
	def exchange(self, index1, index2):
		temp = self.keys[index1]
		self.keys[index1] = self.keys[index2]
		self.keys[index2] = temp
		self.indices[self.keys[index2]] = index2
		self.indices[self.keys[index1]] = index1
	def sort(self):
		original_size = self.size
		for i in range(original_size, 1, -1):
			self.exchange(1, i)
			self.size = self.size - 1
			self.heapify(1)
		self.sorted = True
		if(self.t == "min"):
			self.set_type("max")
		else:
			self.set_type("min")
		self.size = original_size
	def pop(self, key = None):
		if(key == None): index = 1
		else: index = self.indices[key]
		if(index <= self.size and index != 0):
			extracted = self.keys[index]
		else:	
			print "index out of range"
			return
		self.keys[index] = self.keys[self.size]
		self.indices[self.keys[index]] = index
		self.indices[extracted] = 0
		self.size = self.size - 1
		#self.keys = self.keys[0:self.size - 1]
		if(index <= parent(self.size)):
			self.heapify(index)
		return [extracted, self.values[extracted]]
	def insert(self, key, value):
		if(self.indices[key] != 0 ):
			print "key has been in heap, use update instead"
			return
		elif(self.size >= self.max_size):
			print "size overflow"
			return
		else: 
			self.size = self.size + 1
		self.values[key] = value
		self.keys[self.size] = key
		self.indices[key] = self.size
		self.update(key, value)
	def max_update(self, key, value):
		index = self.indices[key]
		if(index < 1 or index > self.size):
			print "no such key in heap"
			return
		if(value < self.values[key]):
			self.values[key] = value
			self.max_heapify(index)
		else:
			self.values[key] = value
			while (index > 1 and self.values[self.keys[parent(index)]] < self.values[self.keys[index]]):
				self.exchange(index, parent(index))
				index = parent(index)
	def max_heapify(self, i):
		l = left(i)
		r = right(i)
		if (l <= self.size and self.values[self.keys[l]] > self.values[self.keys[i]]):
			maximum = l
		else:
			maximum = i
		if (r <= self.size and self.values[self.keys[r]] > self.values[self.keys[maximum]]):
			maximum = r
		if (maximum != i):
			self.exchange(i, maximum)
			self.max_heapify(maximum)
	def min_update(self, key, value):
		index = self.indices[key]
		if(index < 1 or index > self.size):
			print "no such key in heap"
			return
		if(value > self.values[self.keys[index]]):
			self.values[key] = value
			self.min_heapify(index)
		else:
			self.values[key] = value
			while (index > 1 and self.values[self.keys[parent(index)]] > self.values[self.keys[index]]):
				self.exchange(parent(index), index)
				index = parent(index)
	def min_heapify(self, i):
		l = left(i)
		r = right(i)
		if (l <= self.size and self.values[self.keys[l]] < self.values[self.keys[i]]):
			minimum = l
		else:
			minimum = i
		if (r <= self.size and self.values[self.keys[r]] < self.values[self.keys[minimum]]):
			minimum = r
		if (minimum != i):
			self.exchange(i, minimum)
			self.min_heapify(minimum)
	def build_heap(self):
		for i in range(self.size / 2, 0, -1):
			self.heapify(i)
	def has(self, key):
		return not not self.indices[key]
	def p(self, index = 0):
		if(index == 0):
			for i in range(1, self.size + 1):
				self.p(i)
		else:
			print index, self.keys[index], self.indices[self.keys[index]], self.values[self.keys[index]]
def parent(index):
	return index / 2
def left(index):
	return 2 * index
def right(index):
	return 2 * index + 1
