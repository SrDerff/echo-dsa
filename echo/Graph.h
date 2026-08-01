#pragma once
#include <vector>
#include "HashTable.h"
#include "LinkedList.h"

template<typename T>
class Graph {
private:
	struct Edge {
		int from;
		int to;
		int weight;
		Edge(int f, int t, int w) : from(f), to(t), weight(w) {}
	};

	std::vector<T>vertices;

public:

};