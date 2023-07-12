// Distributed under GPLv3 license, see accompanying files LICENSE

#pragma once

#include <stddef.h>


/**
 * container_of - cast a member of a structure out to the containing structure
 *
 * @ptr:	    the pointer to the member.
 * @type:       the type of the container struct this is embedded in.
 * @member:     the name of the member within the struct.
 *
 */
#ifndef container_of
#define container_of(ptr, type, member) ((type *)((char *)(ptr) - offsetof(type, member)))
#endif


/**
 * list_entry - get the struct for this entry
 * @ptr:	the &struct list_head pointer.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_struct within the struct.
 */
#ifndef list_entry
#define list_entry(ptr, type, member) container_of(ptr, type, member)
#endif

 /**
  * list_first_entry - get the first element from a list
  * @ptr:	the list head to take the element from.
  * @type:	the type of the struct this is embedded in.
  * @member:	the name of the list_struct within the struct.
  *
  * Note, that list is expected to be not empty.
  */
#define list_first_entry(ptr, type, member) list_entry((ptr)->next, type, member)


struct list_head {
    list_head* next = NULL;
    list_head* prev = NULL;
};

inline void INIT_LIST_HEAD(list_head* list) {
    list->next = list;
    list->prev = list;
}

/**
 * list_empty - tests whether a list is empty
 * @head: the list to test.
 */
inline int list_empty(const struct list_head* head)
{
    return head->next == head;
}

/**
 * list_replace - replace old entry by new one
 * @old : the element to be replaced
 * @new : the new element to insert
 *
 * If @old was empty, it will be overwritten.
 */
inline void list_replace(list_head* old, list_head* new_)
{
    new_->next = old->next;
    new_->next->prev = new_;
    new_->prev = old->prev;
    new_->prev->next = new_;
}

inline void list_replace_init(list_head* old, list_head* new_) {
    list_replace(old, new_);
    INIT_LIST_HEAD(old);
}

/*
 * Insert a new entry between two known consecutive entries.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
inline void __list_add(list_head* new_, list_head* prev, list_head* next) {
    next->prev = new_;
    new_->next = next;
    new_->prev = prev;
    prev->next = new_;
}

/*
 * Delete a list entry by making the prev/next entries
 * point to each other.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
inline void __list_del(list_head* prev, list_head* next)
{
    next->prev = prev;
    prev->next = next;
}

/**
 * list_add_tail - add a new entry
 * @new: new entry to be added
 * @head: list head to add it before
 *
 * Insert a new entry before the specified head.
 * This is useful for implementing queues.
 */
inline void list_add_tail(struct list_head* new_, struct list_head* head)
{
    __list_add(new_, head->prev, head);
}


/*
 * Architectures might want to move the poison pointer offset
 * into some well-recognized area such as 0xdead000000000000,
 * that is also not mappable by user-space exploits:
 */
#define POISON_POINTER_DELTA 0


/*
 * These are non-NULL pointers that will result in page faults
 * under normal circumstances, used to verify that nobody uses
 * non-initialized list entries.
 */
#define LIST_POISON1  (0x00100100 + POISON_POINTER_DELTA)
#define LIST_POISON2  (0x00200200 + POISON_POINTER_DELTA)
