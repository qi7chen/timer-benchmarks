// Copyright (C) 2021 simon@qchen.fun. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#pragma once

#include <vector>

enum Color
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
    Color color = BLACK;
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
    ~RBTree() { Clear(); }

    RBTree(const RBTree&) = delete;
    RBTree& operator=(const RBTree&) = delete;

    int Size() const {
        return size_;
    }

    void Clear();

    std::pair<V, bool> Get(const K& key) const
    {
        auto p = getEntry(key);
        if (p != nullptr) {
            return std::make_pair(p->value, true);
        }
        return std::make_pair(V(), false);
    }

    bool Contains() const {
        auto p = getEntry(key);
        return p != nullptr;
    }

    bool Remove(const K& key)
    {
        auto p = getEntry(key);
        if (p != nullptr) {
            removeEntry(p);
            deleteEntry(p);
            return true;
        }
        return false;
    }

   
    Entry<K, V>* Put(const K& key, const V& val);

    Entry<K, V>* getEntry(const K& key) const;
    Entry<K, V>* getFirstEntry() const;
    Entry<K, V>* getLastEntry() const;
    void getEntries(std::vector<Entry<K, V>*>& out) const;

    void removeEntry(Entry<K, V>* p);

    Entry<K, V>* allocEntry(const K& key, const V& value, Entry<K, V>* parent);
    Entry<K, V>* deleteEntry(Entry<K, V>* p);

private:
    void rotateLeft(Entry<K,V>* p);
    void rotateRight(Entry<K, V>* p);
    void fixAfterInsertion(Entry<K, V>* p);
    void fixAfterDeletion(Entry<K, V>* p);

private:
    Entry<K, V>* root_ = nullptr;
    int size_ = 0;
};

#include "RBTree-inl.h"
