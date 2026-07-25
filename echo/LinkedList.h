#pragma once

template<typename T>
class LinkedList {
private:
	class Node {
	public:
		T value;
		Node* next;
		Node* prev;
		Node(const T& val) : value(val), next(nullptr), prev(nullptr) {}
	};

	Node* head;
	Node* tail;
	size_t size;

public:
	LinkedList() : head(nullptr), tail(nullptr), size(0) {}

	~LinkedList() { clear(); }

	void pushFront(const T& value) {
		Node* newNode = new Node(value);
		if (isEmpty()) {
			head = tail = newNode;
		}
		else {
			newNode->next = head;
			head->prev = newNode;
			head = newNode;
		}
		++size;
	}

	void pushBack(const T& value) {
		Node* newNode = new Node(value);
		if (isEmpty()) {
			head = tail = newNode;
		}
		else {
			newNode->prev = tail;
			tail->next = newNode;
			tail = newNode;
		}
		++size;
	}

	void popFront() {
		if (isEmpty()) return;
		Node* temp = head;
		head = head->next;
		if (head) head->prev = nullptr;
		else tail = nullptr;
		delete temp;
		--size;
	}

	void popBack() {
		if (isEmpty()) return;
		Node* temp = tail;
		tail = tail->prev;
		if (tail) tail->next = nullptr;
		else head = nullptr;
		delete temp;
		--size;
	}

	void insertAt(size_t index, const T& value) {
		if (index > size) return;
		if (index == 0) { pushFront(value); return; }
		if (index == size) { pushBack(value); return; }

		Node* current = head;
		for (size_t i = 0; i < index; ++i) current = current->next;

		Node* newNode = new Node(value);
		newNode->prev = current->prev;
		newNode->next = current;
		current->prev->next = newNode;
		current->prev = newNode;
		++size;
	}

	void removeAt(size_t index) {
		if (index >= size) return;
		if (index == 0) { popFront(); return; }
		if (index == size - 1) { popBack(); return; }

		Node* current = head;
		for (size_t i = 0; i < index; ++i) current = current->next;

		current->prev->next = current->next;
		current->next->prev = current->prev;
		delete current;
		--size;
	}

	bool remove(const T& value) {
		Node* current = head;
		while (current) {
			if (current->value == value) {
				if (current == head) { popFront(); return true; }
				if (current == tail) { popBack(); return true; }
				current->prev->next = current->next;
				current->next->prev = current->prev;
				delete current;
				--size;
				return true;
			}
			current = current->next;
		}
		return false;
	}

	T& front() {
		return head->value;
	}

	T& back() {
		return tail->value;
	}

	T& getAt(size_t index) {
		Node* current = head;
		for (size_t i = 0; i < index; ++i) current = current->next;
		return current->value;
	}

	size_t getSize() const { return size; }

	bool isEmpty() const { return size == 0; }

	void clear() {
		Node* current = head;
		while (current) {
			Node* next = current->next;
			delete current;
			current = next;
		}
		head = tail = nullptr;
		size = 0;
	}
};
