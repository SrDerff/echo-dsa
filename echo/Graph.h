#pragma once
#include <vector>
#include <stdexcept>
#include "HashTable.h"
#include "LinkedList.h"

template<typename VertexType, typename Weight>
class Graph {
public:
	class Edge {
	public:
		size_t destination;
		Weight weight;
		Edge(size_t _dest, Weight _weight) 
			: destination(_dest), weight(_weight) {}
	};
private:
	std::vector<VertexType>vertices;
	std::vector<std::vector<Edge>> adjacencyList;
	bool directed;
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
					<< vertices[edge.destination]
					<< ", "
					<< edge.weight
					<< ") ";
			}
			std::cout << '\n';
		}
	}
};