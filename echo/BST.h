#pragma once

template<typename T>
class BST {
private:
	class Node {
	public:
		T value;
		Node* left;
		Node* right;
		Node(T val) {
			value = val;
			left = nullptr;
			right = nullptr;
		}
	};

	size_t currSize;
public:

};