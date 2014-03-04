import heap
import graph
import array

def dijkstra(G, s_label, t_label):
	"""
	Using Dijkstra's algorithm to find the MAX_BANDWIDTH from source to destination.
	G is a graph represented by adjacency list, 
	the labels for vertices in which must be consecutive positive integers.
	s_label is the unique label for source vertex
	t_label is the unique label for destination vertex
	"""
	h = heap.Heap(len(G.list), [])
	parent = array.array('I', [0] * (h.max_size + 1))
	i = 0
	index = array.array('I', [0] * (h.max_size + 1))
	for v in G.list:
		h[v.label] = 0
		parent[v.label] = 0
		index[v.label] = i
		i = i + 1
	h[s_label] = graph.MAX_BANDWIDTH 
	while (h.indices[t_label] != 0  ):
		u_label_weight = h.pop()
		if(h[u_label_weight[0]] == 0): break
		u = G.list[index[u_label_weight[0]]]
		for v_label_weight in u.list:
			new_bandwidth = min(h[u_label_weight[0]], v_label_weight[1])
			if(new_bandwidth > h[v_label_weight[0]]):
				h[v_label_weight[0]] = new_bandwidth
				parent[v_label_weight[0]] = u_label_weight[0]
	return [h[t_label], parent]
