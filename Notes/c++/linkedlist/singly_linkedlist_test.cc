
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#if !defined(__cplusplus)
#define nullptr ((void *)0)
#endif

typedef struct linkedlist
{
    struct linkedlist * next;
    int data;
} linkedlist;

static linkedlist * head, * tail;

void reset_list_entry()
{
    head = nullptr;
    tail = nullptr;
}

void init_list_entry()
{
    reset_list_entry();
}

size_t get_list_count() {
    size_t count = 0;
    linkedlist * iter = head;
    while (iter) {
        count++;
        iter = iter->next;
    }
    return count;
}

void display_list_entry() {
    printf("-----------------------------------------\n");
    printf("head: 0x%p\n", head);
    linkedlist * iter = head;
    int i = 0;
    while (iter) {
        printf(">>>>  entry[%3d] - addr: 0x%p, next: 0x%p, data = %d\n", i, iter, iter->next, iter->data);
        i++;
        iter = iter->next;
    }
    printf("tail: 0x%p\n", tail);
    printf("-----------------------------------------\n\n");
}

linkedlist * create_list_entry(int data)
{
    linkedlist * entry = new linkedlist;
    entry->next = nullptr;
    entry->data = data;
    return entry;
}

void append_list_entry(linkedlist * entry)
{
    assert(entry != nullptr);
    entry->next = nullptr;
    if (tail != nullptr) {
        assert(tail->next == nullptr);
        tail->next = entry;
        tail = entry;
    }
    else {
        head = entry;
        tail = entry;
    }
}

bool remove_list_entry(linkedlist * entry)
{
    assert(entry != nullptr);
    if (head == entry) {
        head = head->next;
        if (entry == tail)
            tail = head;
        delete entry;
        return true;
    }

    linkedlist * iter = head;
    do {
        if (iter == nullptr)
            return false;

        if (iter->next != entry)
            iter = iter->next;
        else
            break;
    } while (1);

    iter->next = entry->next;
    if (iter->next == nullptr)
        tail = iter;
    delete entry;
    return true;
}

bool remove_list_entry2(linkedlist * entry)
{
    assert(entry != nullptr);
    if (head == nullptr)
        return false;

    if (head == entry) {
        head = head->next;
        if (entry == tail)
            tail = head;
        delete entry;
        return true;
    }

    linkedlist * iter = head;
    while (iter->next != entry) {
        iter = iter->next;
        if (!iter)
            return false;
    }

    iter->next = entry->next;
    if (iter->next == nullptr)
        tail = iter;
    delete entry;
    return true;
}

bool remove_list_entry_fast(linkedlist * entry)
{
    assert(entry != nullptr);
    linkedlist ** indirect = &head;

    // Notes: *indirect = iter->next, except first time value = &head.
    while ((*indirect) != entry) {
        if ((*indirect) != nullptr) {
            indirect = &((*indirect)->next);
        }
        else {
            return false;
        }
    }

    // Notes: iter->next = entry->next;
    // if (head == entry && head == tail) {
    if (head == tail) {
        tail = entry->next;
    }
    else if (entry->next == nullptr) {
        tail = (linkedlist *)indirect;      // TODO: tail = iter; ???
    }
    (*indirect) = entry->next;
    delete entry;
    return true;
}

void free_list_entry()
{
    linkedlist * entry;
    linkedlist * iter = head;
    while (iter) {
        entry = iter;
        iter = iter->next;
        delete entry;
    }

    head = nullptr;
    tail = nullptr;
}

void test_singly_linkedlist()
{
    init_list_entry();

    linkedlist * node1 = create_list_entry(1);
    append_list_entry(node1);
    linkedlist * node2 = create_list_entry(2);
    append_list_entry(node2);
    linkedlist * node3 = create_list_entry(3);
    append_list_entry(node3);

    printf("append 3 entries;  count = %zu\n", get_list_count());
    display_list_entry();

    linkedlist * node = node3;
    remove_list_entry_fast(node3);

    printf("remove_list_entry(node3);  count = %zu\n", get_list_count());
    display_list_entry();
    
    remove_list_entry_fast(node1);

    printf("remove_list_entry(node1);  count = %zu\n", get_list_count());
    display_list_entry();
    
    remove_list_entry_fast(node2);
    
    printf("remove_list_entry(node2);  count = %zu\n", get_list_count());
    display_list_entry();

    free_list_entry();
    printf("\n");
}

int main(int argc, char * argv[])
{
    test_singly_linkedlist();
#ifdef _WIN32
    ::system("pause");
#endif
    return 0;
}
