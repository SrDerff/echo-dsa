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

	void _add() {

	}
	void _remove() {

	}

	size_t size;
public:
	BST() {

	}
	~BST() {

	}

	void add(){
	
	}
	void remove(){
		
	}

	size_t getSize() { return size; }
};