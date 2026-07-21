#pragma once

#include <vector>

template<typename K, typename V, typename Hash>
class HashTable {
private:
	//hash node class
	class HashNode {
	public:
		K key;
		V value;

		HashNode(const &_key, const&_value) {
			key = _key;
			value = _value;
		}
	};

	//attributes
	std::vector<HashNode*>bucket;
	size_t currSize;
	size_t capacity;
	const float LOAD_FACTOR = 0.75;
	Hash hash;

	//private methods
	void reSize() {
		//copy the actual bucket
		std::vector<HashNode*> oldTable=bucket;

		//clear bucket
		bucket.clear();
		//expand size
		capacity *= 2;
		//resize bucket
		bucket.resize(capacity);

		for (auto& node : oldTable) {
			
		}
	}

public:
	HashTable(Hash hashFunction) : hash(hashFunction) {
		currSize = 0;
		capacity = 101;
		bucket.assign(capacity, nullptr);
	}
	bool insert(K key, V value){
		//get the hashed index
		int index = hash(key) % capacity;

		//check if that element already exists in the bucket
		if (contains(key)) return false;

		//try linear probing if an element already exists in that index
		while (bucket[index] != nullptr) {
			index = (index + 1) % capacity;
		}

		//insert val
		bucket[index]=new HashNode(key, value);

		//check if the size exceeds the load factor
		if ((float)(++currSize)/capacity >= LOAD_FACTOR) reSize();

		return true;
	}
	bool remove(){}
	bool contains(){}
	void clear(){}
	T getElement(){}

	//regular getters
	size_t getSize() { return currSize; }
};