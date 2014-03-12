import random
import array

MAX_BANDWIDTH = 5000

class Vertex:
	"""A Graph Vertex"""
	def __init__(self, label = 0, adjacency_list = []):
		self.label = label
		self.list = list(adjacency_list)
	def add_adjacency_vertex(self, label, weight = 0):
		self.list.append([label, weight])
	def set_adjacency_list(self, adjacency_list):
		self.list = adjacency_list 
	def p(self):
		print self.label , " -> " , self.list

class Graph:
	"""A Graph"""
	def __getitem__(self, label):
		return self.adj_list[label]
	def __setitem__(label, vertex):
		self.adj_list[label] = vertex
	def __init__(self, vertex_list = []):
		self.max_V = len(vertex_list)
		self.V = len(vertex_list)
		self.adj_list = [0] * (len(vertex_list) + 1)
		for v in vertex_list:
			self.adj_list[v.label] = v
	def edge_list(self):
		edges = [[0,0]]
		edge_weight = [-1]
		for v in self.adj_list[1:]:
			for u in v.list:
				if(v.label < u[0]):
					edges.append([v.label, u[0]])
					edge_weight.append(u[1])
		return [edges, edge_weight]
	def p(self):
		for v in self.adj_list[1:]:
			v.p()
def gen(D, V):
	"""
	D is the degree for every vertex
	V is the total number of vertices
	generate a random graph with V vertices, and every vertex has exact degree of D
	"""
	if(D * V > V * (V - 1)):
		print "No such graph: total degree should be less than of equal to 2 x maximal number of edge"
		return Graph([])
	sets = map(list, [[]] * (D + 1))
	sets[0] = map(Vertex, range(1, V + 1))
	min_d = 0
	v1 = Vertex()
	v2 = Vertex()
	while(len(sets[D]) < V - 1 ):
		v = random.randrange (V - len(sets[D]))
		if(v / len(sets[min_d]) == 0):
			v1 = sets[min_d].pop(v)
			if(len(sets[min_d]) == 0):
				min_d = min_d + 1
		else:
			v = v - len(sets[min_d])
			for d in range(min_d + 1, D):
				if(v / len(sets[d]) == 0):
					v1 = sets[d].pop(v)
					break
				else:
					v = v - len(sets[d])
		l = D - len(v1.list)
		d = min_d
		pl = 0
		while(l > 0 and d < D):
			untouched_len = len(sets[d]) - pl 
			if(l < untouched_len):
				for i in range(l):
					v2 = sets[d].pop(random.randrange(len(sets[d]) - pl))
					weight = random.randint(1,MAX_BANDWIDTH)
					v2.add_adjacency_vertex(v1.label, weight)
					v1.add_adjacency_vertex(v2.label, weight)
					if(len(v2.list) == d + 1): 
						sets[d + 1].append(v2)
				l = 0
			else:
				for i in range(untouched_len):
					v2 = sets[d].pop(random.randrange(len(sets[d]) - pl))
					weight = random.randint(1,MAX_BANDWIDTH)
					v2.add_adjacency_vertex(v1.label, weight)
					v1.add_adjacency_vertex(v2.label, weight)
					if(len(v2.list) == d + 1): 
						sets[d + 1].append(v2)
				l = l - untouched_len
				pl = untouched_len
				d = d + 1
		if(len(sets[min_d]) == 0): min_d = min_d + 1
		if(len(v1.list) == D): 
			sets[D].append(v1)
	if(len(sets[D]) == V):
		return Graph(sets[D])
	else:
		print "No such graph: Returned graph has a vertex with degree less than D"
		sets[D].append(sets[D - 1].pop())
		return Graph(sets[D])
