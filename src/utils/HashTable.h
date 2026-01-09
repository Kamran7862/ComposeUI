#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <array>
#include <cstddef> // size_t

/**
 * @brief A fixed-capacity hash table with open addressing (linear probing).
 *
 * @tparam Key Type of the keys.
 * @tparam Value Type of the values.
 * @tparam Capacity Maximum number of slots in the table.
 */
template<typename Key, typename Value, size_t Capacity>
class HashTable {
private:
    struct Entry {
        Key key{};
        Value value{};
        bool occupied = false;  // True if this slot has ever been used.
        bool deleted = false;  // True if this slot was removed (tombstone).
    };

    std::array<Entry, Capacity> table; // Fixed-size array of entries.

    // Compute the hash index for a key.
    size_t hashFunction(const Key& key) const {
        return static_cast<size_t>(key) % Capacity;
    }

public:
    HashTable() = default;

    /**
     * @brief Insert a new key-value pair or update an existing entry.
     *
     * @param key The key to insert or update.
     * @param value The value to associate with the key.
     * @return True if insertion/update succeeded, false if the table is full.
     */
    bool insert(const Key& key, const Value& value) {
        size_t index = hashFunction(key);
        // Probe linearly until we find an empty, deleted, or matching slot.
        for (size_t i = 0; i < Capacity; ++i) {
            size_t probe = (index + i) % Capacity;
            // Place or update entry
            if (!table[probe].occupied || table[probe].deleted || table[probe].key == key) {
                table[probe].key = key;
                table[probe].value = value;
                table[probe].occupied = true;
                table[probe].deleted = false;
                return true;
            }
        }
        return false; // Table is full, insertion failed.
    }

    /**
     * @brief Lookup a key and return a const pointer to its value.
     *
     * This is the non-modifying form of the `find` method.
     * 
     * @param key The key to search for.
     * @return Const pointer to the stored value if found, nullptr otherwise.
     *
     */
    const Value* find(const Key& key) const {
        size_t index = hashFunction(key);
        // Probe linearly until we find the key or exhaust capacity.
        for (size_t i = 0; i < Capacity; ++i) {
            size_t probe = (index + i) % Capacity;
            if (table[probe].occupied && !table[probe].deleted && table[probe].key == key) {
                return &table[probe].value;  // Return pointer to stored value.
            }
        }
        return nullptr;
    }

    /**
     * @brief Lookup a key and return a mutable pointer to its value.
     *
     * This is the modifying form of the `find` method.
     *
     * @param key The key to search for.
     * @return Pointer to the stored value if found, nullptr otherwise.
     *
     */
    Value* find(const Key& key) {
        return const_cast<Value*>(static_cast<const HashTable&>(*this).find(key));
    }

    /**
     * @brief Remove an entry by key.
     *
     * @param key The key to remove.
     * @return true if the entry is found.
     * 
     */
    bool remove(const Key& key) {
        size_t index = hashFunction(key);
        for (size_t i = 0; i < Capacity; ++i) {
            size_t probe = (index + i) % Capacity;
            if (table[probe].occupied && !table[probe].deleted && table[probe].key == key) {
                table[probe].deleted = true;   // Tombstone preserves probe chain.
                table[probe].value = Value{};
                table[probe].key   = Key{};
                return true;
            }
        }
        return false;
    }

    /**
     * @brief Count the number of valid entries in the table.
     * 
     * @return Number of valid entries.
     * 
     */
    size_t getSize() const {
        size_t count = 0;
        for (const auto& entry : table) {
            if (entry.occupied && !entry.deleted) {
                ++count;
            }
        }
        return count;
    }
    
    /**
     * @brief Check whether the table contains any valid entries.
     *
     * @return True if the table is empty.
     * 
     */
    bool isEmpty() const {
        return getSize() == 0;
    }

    // A forward iterator.
    class Iterator {
        private:
            Entry* table; // Pointer to the HashTable array.
            size_t index; // Current index.
            size_t capacity; // Total number of slots in the array (m).

            // Do not return empty or deleted entries from the HashTable.
            void skipInvalid() {
                while (index < capacity &&
                    (!table[index].occupied || table[index].deleted)) {
                    ++index;
                }
            }

        public:
            Iterator(Entry* table, size_t index, size_t capacity)
                : table(table), index(index), capacity(capacity) {
                skipInvalid();
            }

            Entry& operator*() { return table[index]; } // Read/Write.
            Entry* operator->() { return &table[index]; } // Access.

            // Next.
            Iterator& operator++() {
                ++index;
                skipInvalid();
                return *this;
            }

            bool operator!=(const Iterator& other) const {
                return index != other.index;
            }
    };

    Iterator begin() const { return Iterator(const_cast<Entry*>(table.data()), 0, Capacity); }
    Iterator end()   const { return Iterator(const_cast<Entry*>(table.data()), Capacity, Capacity); }
};

#endif