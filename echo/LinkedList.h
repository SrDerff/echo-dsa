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
	class Iterator {
		friend class LinkedList;
	private:
		Node* node;
		Iterator(Node* n) : node(n) {}
	public:
		Iterator& operator++() { node = node->next; return *this; }
		bool operator!=(const Iterator& other) const { return node != other.node; }
		T& operator*() const { return node->value; }
	};

	LinkedList() : head(nullptr), tail(nullptr), size(0) {}

	LinkedList(const LinkedList& other) : head(nullptr), tail(nullptr), size(0) {
		copyFrom(other);
	}

	LinkedList(LinkedList&& other) noexcept
		: head(other.head), tail(other.tail), size(other.size)
	{
		other.head = other.tail = nullptr;
		other.size = 0;
	}

	LinkedList& operator=(const LinkedList& other) {
		if (this != &other) {
			clear();
			copyFrom(other);
		}
		return *this;
	}

	LinkedList& operator=(LinkedList&& other) noexcept {
		if (this != &other) {
			clear();
			head = other.head;
			tail = other.tail;
			size = other.size;
			other.head = other.tail = nullptr;
			other.size = 0;
		}
		return *this;
	}

	~LinkedList() { clear(); }

private:
	void copyFrom(const LinkedList& other) {
		Node* current = other.head;
		while (current) {
			pushBack(current->value);
			current = current->next;
		}
	}

public:
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

	Iterator begin() { return Iterator(head); }
	Iterator end() { return Iterator(nullptr); }

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
