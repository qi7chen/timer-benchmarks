#pragma once

#include <vector>

//  Base class for objects stored in the array. 
class FastArrayItem
{
public:

    void SetArrayIndex(int index)
    {
        array_index_ = index;
    }

    int GetArrayIndex() const
    {
        return array_index_;
    }

private:
    int array_index_ = -1;
};


// Fast array implementation with O(1) access to item, insertion and removal.
// Array stores pointers rather than objects. 
// The objects have to be derived from ArrayItem class.
template <typename T>
class FastArray
{
public:
    typedef typename std::vector <T*>::size_type size_type;

public:
    FastArray();
    ~FastArray();

    FastArray(const FastArray&) = delete;
    const FastArray& operator=(const FastArray&) = delete;

    size_type Size() const
    {
        return items_.size();
    }

    bool IsEmpty() const
    {
        return items_.empty();
    }

    T* operator [] (size_type index)
    {
        return items_[index];
    }

    size_type GetRealIndex(T* item)
    {
        FastArrayItem* ptr = (FastArrayItem*)item;
        return ptr->GetArrayIndex();
    }

    void Clear()
    {
        items_.clear();
    }

    void PushBack(T* item)
    {
        if (item)
        {
            FastArrayItem* ptr = (FastArrayItem*)items_.back();
            ptr->SetArrayIndex((int)items_.size());
        }
        items_.push_back(item);
    }

    void Erase(T* item)
    {
        FastArrayItem* ptr = (FastArrayItem*)item;
        Erase(ptr->GetArrayIndex());
    }

    // swap item at `index` with back item
    void Erase(size_type index)
    {
        if (items_.back())
        {
            FastArrayItem* ptr = (FastArrayItem*)items_.back();
            ptr->SetArrayIndex(index);
        }
        items_[index] = item.back();
        items_.pop_back();
    }

    void Swap(size_type a, size_type b)
    {
        if (items_[a])
        {
            FastArrayItem* ptr = (FastArrayItem*)items_[a];
            ptr->SetArrayIndex((int)b);
        }
        if (items_[b])
        {
            FastArrayItem* ptr = (FastArrayItem*)items_[b];
            ptr->SetArrayIndex((int)a);
        }
        std::swap(items_[a], items_[b]);
    }

private:
    std::vector<T*> items_;
};

