import heap
import graph
import array

def dkt(G, s_label, t_label):
	"""
	Using Dijkstra's algorithm to find the MAX_BANDWIDTH from source to destination.
	G is a graph represented by adjacency list, 
	the labels for vertices in which must be consecutive positive integers.
	s_label is the unique label for source vertex
	t_label is the unique label for destination vertex
	"""
	h = heap.Heap(G.V, [])
	parent = array.array('I', [0] * (h.max_size + 1))
	for v in G.adj_lists[1:]:
		h[v.label] = 0
		parent[v.label] = 0
	h[s_label] = graph.MAX_BANDWIDTH 
	while (h.has(t_label) ):
		u_label_weight = h.pop()
		if(h[u_label_weight[0]] == 0): break
		u = G[u_label_weight[0]]
		for v_label_weight in u.list:
			new_bandwidth = min(h[u_label_weight[0]], v_label_weight[1])
			if(new_bandwidth > h[v_label_weight[0]]):
				h[v_label_weight[0]] = new_bandwidth
				parent[v_label_weight[0]] = u_label_weight[0]
	return [h[t_label], parent]

def dkt_no_heap(G, s_label, t_label):
	h = array.array('I', [0]* (G.V + 1))
	s = array.array('c', ['0']* (G.V + 1))
	parent = array.array('I', [0] * (G.V + 1))
	for v in G.adj_lists[1:]:
		h[v.label] = 0
		s[v.label] = '1'
		parent[v.label] = 0
	h[s_label] = graph.MAX_BANDWIDTH
	while (s[t_label] == '1'):
		max_w = h[0]
		max_w_i = 0 
		for i in range(1,G.V+1):
			if(s[i] == '1' and h[i] > max_w):
				max_w = h[i]
				max_w_i = i
		u_label_weight = [max_w_i, max_w];
		s[u_label_weight[0]] = '0'
		if(h[u_label_weight[0]] == 0): break
		u = G[u_label_weight[0]]
		for v_label_weight in u.list:
			new_bandwidth = min(h[u_label_weight[0]], v_label_weight[1])
			if(new_bandwidth > h[v_label_weight[0]]):
				h[v_label_weight[0]] = new_bandwidth
				parent[v_label_weight[0]] = u_label_weight[0]
	return [h[t_label], parent]
 	