// Copyright (C) 2021 simon@qchen.fun. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#pragma once

#include "RBTree.h"

template <typename K, typename V>
inline Entry<K, V>* parentOf(Entry<K, V>* p)
{
    if (p != nullptr) {
        return p->parent;
    }
    return nullptr;
}

template <typename K, typename V>
inline Entry<K, V>* leftOf(Entry<K, V>* p)
{
    if (p != nullptr) {
        return p->left;
    }
    return nullptr;
}

template <typename K, typename V>
inline Entry<K, V>* rightOf(Entry<K, V>* p)
{
    if (p != nullptr) {
        return p->right;
    }
    return nullptr;
}

template <typename K, typename V>
inline Color colorOf(Entry<K, V>* p)
{
    if (p != nullptr) {
        return p->color;
    }
    return BLACK;
}

template <typename K, typename V>
inline void setColor(Entry<K, V>* p, Color color)
{
    if (p != nullptr) {
        p->color = color;
    }
}

template <typename K, typename V>
inline const K& key(Entry<K, V>* p)
{
    if (p != nullptr) {
        return p->key;
    }
    return K();
}

// Returns the successor of the specified Entry, or null if no such.
template <typename K, typename V>
inline Entry<K, V>* successor(Entry<K, V>* t)
{
    if (t == nullptr) {
        return nullptr;
    }
    else if (t->right != nullptr) {
        auto p = t->right;
        while (p->left != nullptr) {
            p = p->left;
        }
        return p;
    }
    else {
        auto p = t->parent;
        auto ch = t;
        while (p != nullptr && ch == p->right) {
            ch = p;
            p = p->parent;
        }
        return p;
    }
}

// Returns the predecessor of the specified Entry, or null if no such.
template <typename K, typename V>
inline Entry<K, V>* predecessor(Entry<K, V>* t)
{
    if (t == nullptr) {
        return nullptr;
    }
    else if (t->left != nullptr) {
        auto p = t->left;
        while (p->right != nullptr) {
            p = p->right;
        }
        return p;
    }
    else {
        auto p = t->parent;
        auto ch = t;
        while (p != nullptr && ch == p->left) {
            ch = p;
            p = p->parent;
        }
        return p;
    }

}

template <typename K, typename V>
Entry<K, V>* Entry<K, V>::next() {
    return successor(this);
}

template <typename K, typename V>
void RBTree<K, V>::Clear()
{
    std::vector<Entry<K, V>*> entries;
    getEntries(entries);
    root_ = nullptr;
    size_ = 0;
    for (int i = 0; i < entries.size(); i++)
    {
        freeEntry(entries[i]);
    }
}

template <typename K, typename V>
Entry<K, V>* RBTree<K, V>::Put(const K& key, const V& val)
{
    auto t = root_;
    if (t == nullptr) {
        auto entry = allocEntry(key, val, nullptr);
        size_ = 1;
        root_ = entry;
        return entry;
    }
    int cmp = 0;
    Entry<K, V>* parent = nullptr;
    do {
        parent = t;
        if (key < t->key) {
            t = t->left;
            cmp = -1;
        }
        else if (t->key < key) {
            t = t->right;
            cmp = 1;
        }
        else {
            t->value = val;
            return t;
        }
    } while (t != nullptr);

    auto e = allocEntry(key, val, parent);
    if (cmp < 0) {
        parent->left = e;
    }
    else {
        parent->right = e;
    }
    fixAfterInsertion(e);
    size_++;
    return e;
}

template <typename K, typename V>
Entry<K, V>* RBTree<K, V>::getEntry(const K& key) const
{
    auto node = root_;
    int height = 0;
    while (node != nullptr) {
        if (key < node->key) {
            node = node->left;
        }
        else if (node->key < key) {
            node = node->right;
        }
        else {
            return node;
        }
        height++;
    }
    return nullptr;
}

template <typename K, typename V>
void RBTree<K, V>::getEntries(std::vector<Entry<K, V>*>& entries) const
{
    entries.resize(entries.size() + Size());
    auto node = getFirstEntry();
    while (node != nullptr) {
        auto next = successor(node);
        entries.push_back(node);
        node = next;
    }
}

template <typename K, typename V>
Entry<K, V>* RBTree<K, V>::getFirstEntry() const
{
    auto node = root_;
    if (node != nullptr) {
        while (node->left != nullptr) {
            node = node->left;
        }
    }
    return node;
}

template <typename K, typename V>
Entry<K, V>* RBTree<K, V>::getLastEntry() const
{
    auto node = root_;
    if (node != nullptr) {
        while (node->right != nullptr) {
            node = node->right;
        }
    }
    return node;
}

template <typename K, typename V>
void RBTree<K, V>::removeEntry(Entry<K, V>* p)
{
    size_--;

    // If strictly internal, copy successor's element to p and then make p
   // point to successor.
    if (p->left != nullptr && p->right != nullptr) {
        auto s = successor(p);
        p->key = s->key;
        p->value = s->value;
        p = s;
    }  // p has 2 children

    // Start fixup at replacement node, if it exists.
    auto replacement = p->left;
    if (p->left == nullptr) {
        replacement = p->right;
    }

    if (replacement != nullptr) {
        // Link replacement to parent
        replacement->parent = p->parent;
        if (p->parent == nullptr) {
            root_ = replacement;
        }
        else if (p == p->parent->left) {
            p->parent->left = replacement;
        }
        else {
            p->parent->right = replacement;
        }

        // Null out links so they are OK to use by fixAfterDeletion.
        p->left = nullptr;
        p->right = nullptr;
        p->parent = nullptr;

        // Fix replacement
        if (p->color == BLACK) {
            fixAfterDeletion(replacement);
        }
    }
    else if (p->parent == nullptr) { // return if we are the only node.
        root_ = nullptr;
    }
    else { //  No children. Use self as phantom replacement and unlink.
        if (p->color == BLACK) {
            fixAfterDeletion(p);
        }
        if (p->parent != nullptr) {
            if (p == p->parent->left) {
                p->parent->left = nullptr;
            }
            else if (p == p->parent->right) {
                p->parent->right = nullptr;
            }
            p->parent = nullptr;
        }
    }
}

template <typename K, typename V>
Entry<K, V>* RBTree<K, V>::allocEntry(const K& key, const V& value, Entry<K, V>* parent)
{
    return new Entry<K, V>(key, value, parent);
}

template <typename K, typename V>
Entry<K, V>* RBTree<K, V>::freeEntry(Entry<K, V>* p)
{
    delete p;
    return nullptr;
}


/**
 * Balancing operations.
 *
 * Implementations of rebalancings during insertion and deletion are
 * slightly different than the CLR version.  Rather than using dummy
 * nil nodes, we use a set of accessors that deal properly with nil.  They
 * are used to avoid messiness surrounding nullness checks in the main
 * algorithms.
 */

template <typename K, typename V>
void RBTree<K, V>::rotateLeft(Entry<K, V>* p)
{
    if (p == nullptr) {
        return;
    }
    auto r = p->right;
    p->right = r->left;
    if (r->left != nullptr) {
        r->left->parent = p;
    }
    r->parent = p->parent;
    if (p->parent == nullptr) {
        root_ = r;
    }
    else if (p->parent->left == p) {
        p->parent->left = r;
    }
    else {
        p->parent->right = r;
    }
    r->left = p;
    p->parent = r;
}

template <typename K, typename V>
void RBTree<K, V>::rotateRight(Entry<K, V>* p)
{
    if (p == nullptr) {
        return;
    }
    auto l = p->left;
    p->left = l->right;
    if (l->right != nullptr) {
        l->right->parent = p;
    }
    l->parent = p->parent;
    if (p->parent == nullptr) {
        root_ = l;
    }
    else if (p->parent->right == p) {
        p->parent->right = l;
    }
    else {
        p->parent->left = l;
    }
    l->right = p;
    p->parent = l;
}

template <typename K, typename V>
void RBTree<K, V>::fixAfterInsertion(Entry<K, V>* x)
{
    x->color = RED;
    while (x != nullptr && x != root_ && x->parent->color == RED) {
        if (parentOf(x) == leftOf(parentOf(parentOf(x)))) {
            auto y = rightOf(parentOf(parentOf(x)));
            if (colorOf(y) == RED) {
                setColor(parentOf(x), BLACK);
                setColor(y, BLACK);
                setColor(parentOf(parentOf(x)), RED);
                x = parentOf(parentOf(x));
            }
            else {
                if (x == rightOf(parentOf(x))) {
                    x = parentOf(x);
                    rotateLeft(x);
                }
                setColor(parentOf(x), BLACK);
                setColor(parentOf(parentOf(x)), RED);
                rotateRight(parentOf(parentOf(x)));
            }
        }
        else {
            auto y = leftOf(parentOf(parentOf(x)));
            if (colorOf(y) == RED) {
                setColor(parentOf(x), BLACK);
                setColor(y, BLACK);
                setColor(parentOf(parentOf(x)), RED);
                x = parentOf(parentOf(x));
            }
            else {
                if (x == leftOf(parentOf(x))) {
                    x = parentOf(x);
                    rotateRight(x);
                }
                setColor(parentOf(x), BLACK);
                setColor(parentOf(parentOf(x)), RED);
                rotateLeft(parentOf(parentOf(x)));
            }
        }
    }
    root_->color = BLACK;
}

template <typename K, typename V>
void RBTree<K, V>::fixAfterDeletion(Entry<K, V>* x)
{
    while (x != root_ && colorOf(x) == BLACK) {
        if (x == leftOf(parentOf(x))) {
            auto sib = rightOf(parentOf(x));

            if (colorOf(sib) == RED) {
                setColor(sib, BLACK);
                setColor(parentOf(x), RED);
                rotateLeft(parentOf(x));
                sib = rightOf(parentOf(x));
            }

            if (colorOf(leftOf(sib)) == BLACK &&
                colorOf(rightOf(sib)) == BLACK) {
                setColor(sib, RED);
                x = parentOf(x);
            }
            else {
                if (colorOf(rightOf(sib)) == BLACK) {
                    setColor(leftOf(sib), BLACK);
                    setColor(sib, RED);
                    rotateRight(sib);
                    sib = rightOf(parentOf(x));
                }
                setColor(sib, colorOf(parentOf(x)));
                setColor(parentOf(x), BLACK);
                setColor(rightOf(sib), BLACK);
                rotateLeft(parentOf(x));
                x = root_;
            }
        }
        else { // symmetric
            auto sib = leftOf(parentOf(x));

            if (colorOf(sib) == RED) {
                setColor(sib, BLACK);
                setColor(parentOf(x), RED);
                rotateRight(parentOf(x));
                sib = leftOf(parentOf(x));
            }

            if (colorOf(rightOf(sib)) == BLACK &&
                colorOf(leftOf(sib)) == BLACK) {
                setColor(sib, RED);
                x = parentOf(x);
            }
            else {
                if (colorOf(leftOf(sib)) == BLACK) {
                    setColor(rightOf(sib), BLACK);
                    setColor(sib, RED);
                    rotateLeft(sib);
                    sib = leftOf(parentOf(x));
                }
                setColor(sib, colorOf(parentOf(x)));
                setColor(parentOf(x), BLACK);
                setColor(leftOf(sib), BLACK);
                rotateRight(parentOf(x));
                x = root_;
            }
        }
    }
    setColor(x, BLACK);
}
