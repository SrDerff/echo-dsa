#pragma once
#include <iostream>
#include <vector>
#include <stdexcept>
#include <queue>
#include "HashTable.h"
#include "LinkedList.h"

template<typename VertexType, typename Weight>
class Graph {
public:
	class Edge {
	public:
		size_t to;
		Weight weight;
		Edge(size_t _dest, Weight _weight) 
			: to(_dest), weight(_weight) {}
	};
private:
	std::vector<VertexType>vertices;
	std::vector<std::vector<Edge>> adjacencyList;
	bool directed;

	void _dfs(size_t index, std::vector<bool>& visited) {
		if (visited[index])
			return;

		visited[index] = true;

		//could be replaced by another funcionality
		std::cout << vertices[index] << " ";

		for (const Edge& edge : adjacencyList[index]) {
			_dfs(edge.to, visited);
		}
	}
public:
	Graph(bool dir=false) : directed(dir) {}
	~Graph() {}

	//VERTICES 
	
	size_t addVertex(const VertexType& vertex) {
		vertices.emplace_back(vertex);
		adjacencyList.emplace_back();
		return vertices.size() - 1;
	}

	const VertexType& getVertex(size_t index) const {
		if (index >= vertices.size())
			throw std::out_of_range("Vertex index out of range");

		return vertices[index];
	}

	size_t countVertices() const {
		return vertices.size();
	}

	//EDGES

	void addEdge(size_t from, size_t to, Weight weight) {
		if (from >= vertices.size() || to >= vertices.size())
			throw std::out_of_range("Vertex index out of range");
		adjacencyList[from].emplace_back(to, weight);
		if (!directed) {
			adjacencyList[to].emplace_back(from, weight);
		}
	}

	const std::vector<Edge>& neighbors(size_t vertexIndex) const {
		if (vertexIndex >= vertices.size())
			throw std::out_of_range("Vertex index out of range");

		return adjacencyList[vertexIndex];
	}

	// PRINT

	void print() const {
		for (size_t i = 0; i < vertices.size(); ++i) {
			std::cout << vertices[i] << " -> ";
			for (const Edge& edge : adjacencyList[i]) {
				std::cout
					<< "("
					<< vertices[edge.to]
					<< ", "
					<< edge.weight
					<< ") ";
			}
			std::cout << '\n';
		}
	}

	// DFS: Depth First Search
	// recursive
	void dfs(size_t index) {
		if (index >= vertices.size())
			throw std::out_of_range("invalid index");

		std::vector<bool> visited(vertices.size(), false);

		_dfs(index, visited);
	}

	// BFS: Breadth First Search
	// non-recursive
	void bfs(size_t index) {
		if (index >= vertices.size())
			throw std::out_of_range("invalid index");

		std::vector<bool>visited(vertices.size(), false);
		std::queue<size_t>q;

		q.push(index);
		visited[index] = true;

		while (!q.empty()) {
			size_t current = q.front();
			q.pop();

			//could be replaced by another funcionality
			std::cout << vertices[current] << " ";

			for (const Edge& edge : adjacencyList[current]) {
				if (!visited[edge.to]) {
					visited[edge.to] = true;
					q.push(edge.to);
				}
			}
		}
	}
};