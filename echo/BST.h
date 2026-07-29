#pragma once
#include <utility>

template<typename T, typename Comp, typename Proc>
class BST {
private:
	class Node {
	public:
		T value;
		Node* left;
		Node* right;
		Node(const T& val) {
			value = val;
			left = nullptr;
			right = nullptr;
		}
	};

	Node* root;
	Comp compare;
	Proc process;
	size_t size;

	void _add(Node*& node, const T& elem) {
		if (node == nullptr) {
			node = new Node(elem);
			++size;
			return;
		}
		if (compare(elem, node->value) > 0) _add(node->right, elem);
		else _add(node->left, elem);
	}
	bool _remove(Node*& node, const T& elem) {
		if (node == nullptr) return false;
		if (compare(elem, node->value) > 0) {
			return _remove(node->right, elem);
		}
		else if (compare(elem, node->value) < 0) {
			return _remove(node->left, elem);
		}
		else {
			if (node->left == nullptr && node->right == nullptr) {
				delete node;
				node = nullptr;
				--size;
			}
			else if (node->left == nullptr && node->right != nullptr) {
				Node* copy = node;
				node=node->right;
				delete copy;
				copy = nullptr;
				--size;
			}
			else if (node->left != nullptr && node->right == nullptr) {
				Node* copy = node;
				node = node->left;
				delete copy;
				copy = nullptr;
				--size;
			}
			else {
				Node* maxLeft=node->left;
				while (maxLeft->right != nullptr) {
					maxLeft = maxLeft->right;
				}
				node->value = maxLeft->value;
				_remove(node->left, maxLeft->value);
			}
			return true;
		}
	}	

	void _preOrder(Node* node) {
		if (node == nullptr) return;
		process(node->value);
		_preOrder(node->left);
		_preOrder(node->right);
	}
	void _inOrder(Node* node) {
		if (node == nullptr) return;
		_inOrder(node->left);
		process(node->value);
		_inOrder(node->right);
	}
	void _postOrder(Node* node) {
		if (node == nullptr) return;
		_postOrder(node->left);
		_postOrder(node->right);
		process(node->value);
	}

	bool _search(Node* node, const T& elem) {
		if (node == nullptr) return false;
		if (compare(elem, node->value) > 0) {
			return _search(node->right, elem);
		}
		else if (compare(elem, node->value) < 0) {
			return _search(node->left, elem);
		}
		else return true;
	}

	void _clear(Node*& node) {
		if (node == nullptr) return;
		_clear(node->left);
		_clear(node->right);
		delete node;
		node = nullptr;
	}

public:
	BST(Comp cmp, Proc pro) : compare(cmp), process(pro) {
		root = nullptr;
		size = 0;
	}
	~BST() {
		_clear(root);
	}

	void add(const T& elem){
		_add(root, elem);
	}

	bool remove(const T& elem){
		return _remove(root, elem);
	}	

	void preOrder(){
		_preOrder(root);
	}
	void inOrder(){
		_inOrder(root);
	}
	void postOrder(){
		_postOrder(root);
	}

	bool search(const T& elem) {
		return _search(root, elem);
	}

	// 3/5 rule
	Node* _copy(Node* node) {
		if (node == nullptr) return nullptr;

		Node* newNode = new Node(node->value);
		newNode->left = _copy(node->left);
		newNode->right = _copy(node->right);

		return newNode;
	}

	BST(const BST& other)
		: compare(other.compare),
		process(other.process),
		size(other.size)
	{
		root = _copy(other.root);
	}

	BST& operator=(const BST& other) {
		if (this != &other) {
			_clear(root);

			compare = other.compare;
			process = other.process;
			size = other.size;

			root = _copy(other.root);
		}
		return *this;
	}

	BST(BST&& other) noexcept
		: root(other.root),
		compare(std::move(other.compare)),
		process(std::move(other.process)),
		size(other.size)
	{
		other.root = nullptr;
		other.size = 0;
	}

	BST& operator=(BST&& other) noexcept {
		if (this != &other) {
			_clear(root);

			root = other.root;
			compare = std::move(other.compare);
			process = std::move(other.process);
			size = other.size;

			other.root = nullptr;
			other.size = 0;
		}

		return *this;
	}

	void clear() { _clear(root); size = 0; }
	size_t getSize() { return size; }
	bool empty() { return size==0; }
};