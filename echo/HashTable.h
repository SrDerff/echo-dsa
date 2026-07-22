#pragma once

#include <vector>
#include <optional>
#include <utility>

template<typename K, typename V, typename Hash = std::hash<K>>
class HashTable {
private:
    enum class STATE {
        EMPTY,
        DELETED,
        OCCUPIED
    };

    struct HashNode {
        K key;
        V value;
        STATE state = STATE::EMPTY;

        HashNode(const K& key, const V& value)
            : key(key), value(value), state(STATE::OCCUPIED) {}

        HashNode() = default;
    };

    std::vector<std::optional<HashNode>> bucket;
    size_t currSize;
    size_t capacity;
    size_t deletedCount;
    static constexpr float LOAD_FACTOR = 0.75f;
    Hash hash;

    void rehash() {
        std::vector<std::optional<HashNode>> oldTable = std::move(bucket);
        capacity *= 2;
        bucket.assign(capacity, std::nullopt);
        currSize = 0;
        deletedCount = 0;

        for (auto& node : oldTable) {
            if (!node.has_value()) continue;
            if (node->state != STATE::OCCUPIED) continue;
            insert(node->key, node->value);
        }
    }

        size_t findIndex(const K& key) const {
            size_t index = hash(key) % capacity;
            size_t probed = 0;

            while (probed < capacity) {
                if (!bucket[index].has_value())
                    return capacity;

                if (bucket[index]->state == STATE::OCCUPIED &&
                    bucket[index]->key == key)
                {
                    return index;
                }

                index = (index + 1) % capacity;
                ++probed;
            }

            return capacity;
        }

public:
    HashTable(Hash hashFunction = Hash())
        : currSize(0), capacity(101), deletedCount(0), hash(hashFunction)
    {
        bucket.resize(capacity);
    }

    bool insert(const K& key, const V& value) {
        if ((float)(currSize + deletedCount) / capacity >= LOAD_FACTOR)
            rehash();

        size_t index = hash(key) % capacity;
        size_t firstDeleted = capacity;
        size_t probed = 0;

        while (bucket[index].has_value() && probed < capacity) {
            if (bucket[index]->state == STATE::OCCUPIED &&
                bucket[index]->key == key)
            {
                bucket[index]->value = value;
                return true;
            }

            if (bucket[index]->state == STATE::DELETED &&
                firstDeleted == capacity)
            {
                firstDeleted = index;
            }

            index = (index + 1) % capacity;
            ++probed;
        }

        if (firstDeleted != capacity) {
            index = firstDeleted;
            --deletedCount;
        }

        bucket[index] = HashNode(key, value);
        ++currSize;

        return true;
    }

    V& getElement(const K& key) {
        size_t index = findIndex(key);
        return (bucket[index])->value;
    }

    bool remove(const K& key) {
        size_t index = findIndex(key);
        if (index == capacity)
            return false;

        bucket[index]->state = STATE::DELETED;
        --currSize;
        ++deletedCount;
        return true;
    }

    bool contains(const K& key) const {
        return findIndex(key) != capacity;
    }

    void clear() {
        bucket.assign(capacity, std::nullopt);
        currSize = 0;
        deletedCount = 0;
    }

    std::optional<std::pair<K, V>> getElement(const K& key) const {
        size_t index = findIndex(key);
        if (index == capacity)
            return std::nullopt;
        return std::make_pair(bucket[index]->key, bucket[index]->value);
    }

    size_t getSize() const { return currSize; }
    size_t getCapacity() const { return capacity; }
    bool isEmpty() const { return currSize == 0; }
};
