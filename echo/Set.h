#pragma once

#include <vector>
#include <optional>
#include <functional>

template<typename T, typename Hash = std::hash<T>>
class Set {
private:
    enum class STATE {
        EMPTY,
        DELETED,
        OCCUPIED
    };

    struct SetNode {
        T value;
        STATE state = STATE::EMPTY;

        SetNode() = default;

        SetNode(const T& val)
            : value(val), state(STATE::OCCUPIED) {}
    };

    std::vector<std::optional<SetNode>> bucket;
    size_t currSize;
    size_t capacity;
    size_t deletedCount;
    static constexpr float LOAD_FACTOR = 0.75f;
    Hash hash;

    void rehash() {
        std::vector<std::optional<SetNode>> oldTable = std::move(bucket);
        capacity *= 2;
        bucket.assign(capacity, std::nullopt);
        currSize = 0;
        deletedCount = 0;

        for (auto& node : oldTable) {
            if (!node.has_value()) continue;
            if (node->state != STATE::OCCUPIED) continue;
            insert(node->value);
        }
    }

    size_t findIndex(const T& value) const {
        size_t index = hash(value) % capacity;
        size_t probed = 0;

        while (probed < capacity) {
            if (!bucket[index].has_value())
                return capacity;

            if (bucket[index]->state == STATE::OCCUPIED &&
                bucket[index]->value == value)
            {
                return index;
            }

            index = (index + 1) % capacity;
            ++probed;
        }

        return capacity;
    }

public:
    class Iterator {
        friend class Set;
    private:
        const std::vector<std::optional<SetNode>>* bucketPtr;
        size_t capacity;
        size_t index;

        Iterator(const std::vector<std::optional<SetNode>>* b, size_t cap, size_t idx)
            : bucketPtr(b), capacity(cap), index(idx)
        {
            advanceToOccupied();
        }

        void advanceToOccupied() {
            while (index < capacity) {
                if ((*bucketPtr)[index].has_value() &&
                    (*bucketPtr)[index]->state == STATE::OCCUPIED)
                    return;
                ++index;
            }
        }

    public:
        Iterator& operator++() {
            ++index;
            advanceToOccupied();
            return *this;
        }

        bool operator!=(const Iterator& other) const {
            return index != other.index;
        }

        const T& operator*() const {
            return (*bucketPtr)[index]->value;
        }
    };

    Set(Hash hashFunction = Hash())
        : currSize(0), capacity(101), deletedCount(0), hash(hashFunction)
    {
        bucket.resize(capacity);
    }

    bool insert(const T& value) {
        if ((float)(currSize + deletedCount) / capacity >= LOAD_FACTOR)
            rehash();

        size_t index = hash(value) % capacity;
        size_t firstDeleted = capacity;
        size_t probed = 0;

        while (bucket[index].has_value() && probed < capacity) {
            if (bucket[index]->state == STATE::OCCUPIED &&
                bucket[index]->value == value)
            {
                return false;
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

        bucket[index] = SetNode(value);
        ++currSize;
        return true;
    }

    bool remove(const T& value) {
        size_t index = findIndex(value);
        if (index == capacity)
            return false;

        bucket[index]->state = STATE::DELETED;
        --currSize;
        ++deletedCount;
        return true;
    }

    bool contains(const T& value) const {
        return findIndex(value) != capacity;
    }

    void clear() {
        bucket.assign(capacity, std::nullopt);
        currSize = 0;
        deletedCount = 0;
    }

    size_t getSize() const { return currSize; }
    size_t getCapacity() const { return capacity; }
    bool isEmpty() const { return currSize == 0; }

    Iterator begin() const {
        return Iterator(&bucket, capacity, 0);
    }

    Iterator end() const {
        return Iterator(&bucket, capacity, capacity);
    }
};
