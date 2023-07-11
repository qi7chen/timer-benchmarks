// Copyright (C) 2021 ichenq@gmail.com. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#pragma once

#include <vector>

enum class Color
{
    RED = 0,
    BLACK = 1,
};

template <typename K, typename V>
struct Entry
{
    Entry* left = nullptr;
    Entry* right = nullptr;
    Entry* parent = nullptr;
    Color color = Color::BLACK;
    K key;
    V value;

    Entry(const K& k, const V& v, Entry* parent)
        : key(k), value(v), parent(parent)
    {
    }

    Entry* next();
};

// A Red-Black tree implementation.
// more details see [java.util.TreeMap]
// https://github.com/openjdk-mirror/jdk7u-jdk/blob/master/src/share/classes/java/util/TreeMap.java
template <typename K, typename V>
class RBTree
{
public:
    RBTree() {}

    ~RBTree() {
        clear();
    }

    // disallow copy and assign
    RBTree(const RBTree&) = delete;
    RBTree& operator=(const RBTree&) = delete;

    int size() const {
        return size_;
    }

    std::pair<V, bool> get(const K& key) const
    {
        auto p = getEntry(key);
        if (p != nullptr) {
            return std::make_pair(p->value, true);
        }
        return std::make_pair(V(), false);
    }

    bool contains(const K& key) const {
        auto p = getEntry(key);
        return p != nullptr;
    }

    Entry<K, V>* getEntry(const K& key) const;
    Entry<K, V>* getFirstEntry() const;
    Entry<K, V>* getLastEntry() const;
    void getEntries(std::vector<Entry<K, V>*>& out) const;

    void put(const K& key, const V& val);
    bool putIfAbsent(const K& key, const V& val);

    bool remove(const K& key)
    {
        auto p = getEntry(key);
        if (p != nullptr) {
            removeEntry(p);
            freeEntry(p);
            return true;
        }
        return false;
    }

    void removeEntry(Entry<K, V>* p);
    void clear();

private:
    Entry<K, V>* allocEntry(const K& key, const V& value, Entry<K, V>* parent);
    Entry<K, V>* freeEntry(Entry<K, V>* p);

    void addEntry(const K& key, const V& val, Entry<K, V>* parent, bool add_left);
    void addEntryToEmptyMap(const K& key, const V& val);
    bool putEntry(const K& key, const V& val, bool replace);

    void rotateLeft(Entry<K,V>* p);
    void rotateRight(Entry<K, V>* p);
    void fixAfterInsertion(Entry<K, V>* p);
    void fixAfterDeletion(Entry<K, V>* p);

private:
    Entry<K, V>* root_ = nullptr;
    int size_ = 0;      // number of entries in the tree
    int version_ = 0;   // number of structural modifications to the tree.
};

#include "RBTree-inl.h"
