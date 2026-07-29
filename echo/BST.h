#pragma once

template<typename T, typename Comp>
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

	Comp comp;

	void _add(Node* node, T elem) {
		if (comp());
		//recurisve
	}
	void _remove(T elem) {

	}

	size_t size;
public:
	BST(Comp cmp) : comp(cmp) {

	}
	~BST() {

	}

	void add(){
	
	}
	void remove(){
		
	}

	size_t getSize() { return size; }
};