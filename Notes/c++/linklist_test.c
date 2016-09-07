
#include <stdio.h>
#include <stdlib.h>

typedef struct linklist
{
    linklist * next;
    int data;
} linklist;

static linklist * head, * tail;

void init_list_entry()
{
    head = nullptr;
    tail = nullptr;
}

size_t get_list_count() {
    size_t count = 0;
    linklist * iter = head;
    while (iter) {
        count++;
        iter = iter->next;
    }
    return count;
}

void display_list_entry() {
    printf("-----------------------------------\n");
    printf("head: 0x%p\n", head);
    linklist * iter = head;
    while (iter) {
        printf(">>>> self: 0x%p, next: 0x%p, data = %d\n", iter, iter->next, iter->data);
        iter = iter->next;
    }
    printf("tail: 0x%p\n", tail);
    printf("-----------------------------------\n\n");
}

linklist * create_list_entry(int data)
{
    linklist * entry = new linklist;
    entry->next = nullptr;
    entry->data = data;
    return entry;
}

void append_list_entry(linklist * entry)
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

bool remove_list_entry(linklist * entry)
{
    assert(entry != nullptr);
    if (head == entry) {
        head = head->next;
        if (entry == tail)
            tail = head;
        delete entry;
        return true;
    }

    linklist * iter = head;
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

bool remove_list_entry2(linklist * entry)
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

    linklist * iter = head;
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

void free_list_entry()
{
    linklist * entry;
    linklist * iter = head;
    while (iter) {
        entry = iter;
        iter = iter->next;
        delete entry;
    }

    head = nullptr;
    tail = nullptr;
}

void test_linklist()
{
    init_list_entry();

    linklist * node1 = create_list_entry(1);
    append_list_entry(node1);
    linklist * node2 = create_list_entry(2);
    append_list_entry(node2);
    linklist * node3 = create_list_entry(3);
    append_list_entry(node3);

    display_list_entry();

    linklist * node = node3;
    remove_list_entry(node3);
    display_list_entry();
    printf("remove_list_entry(node3);  count = %zu\n", get_list_count());

    remove_list_entry(node1);
    display_list_entry();
    printf("remove_list_entry(node1);  count = %zu\n", get_list_count());

    remove_list_entry(node2);
    display_list_entry();
    printf("remove_list_entry(node2);  count = %zu\n", get_list_count());

    free_list_entry();
    printf("\n");
}

int main(int argc, char * argv[])
{
    test_linklist();
    return 0;
}
