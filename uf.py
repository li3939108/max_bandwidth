def union(x, y):
	 xRoot = find(x)
	 yRoot = find(y)
	 if xRoot.rank > yRoot.rank:
		 yRoot.parent = xRoot
	 elif xRoot.rank < yRoot.rank:
		 xRoot.parent = yRoot
	 elif xRoot != yRoot: # Unless x and y are already in same set, merge them
		 yRoot.parent = xRoot
		 xRoot.rank = xRoot.rank + 1

def find(x):
	 if x.parent == x:
		return x
	 else:
		x.parent = find(x.parent)
		return x.parent
