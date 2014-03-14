#! /usr/bin/env python

import graph, maxbw, random, time

V = 500
D = 200
graph.MAX_BANDWIDTH = 100


def runtime(method, G, s_label, t_label):
	s = time.time()
	ret = method(G, s_label, t_label)
	e = time.time()
	print\
			method.__name__, 'result: ',\
			ret[0], e - s ,\
			'    Only for Dijkstra    ',\
			' updated edges: ',ret[2], '(',(ret[2] * 100.0) / (V * D / 2),'% )',\
			 ' visited vertices: ',ret[3], '(', (ret[3] * 100.0) / V,'% )'


G = graph.gen(D, V)
G.edges = G.edge_list()
randpair = random.sample(range(1, V + 1), 2)
print 'vertices: ', V
print 'edges: ', D*V/2
print 'MAX_BANDWIDTH: ', graph.MAX_BANDWIDTH
print 'MST: ', (V - 1 + 0.0) / (V*D/2)

#runtime(maxbw.dkt_no_heap, G, randpair[0], randpair[1])
runtime(maxbw.dkt, G, randpair[0], randpair[1])
#runtime(maxbw.krsk, G, randpair[0], randpair[1])
