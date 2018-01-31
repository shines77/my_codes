
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#if !defined(__cplusplus)
#define nullptr ((void *)0)
#endif

typedef struct linkedlist_entry
{
    struct linkedlist_entry * next;
    int value;
} linkedlist_entry;

static linkedlist_entry * head, * tail;

void reset_linkedlist()
{
    head = nullptr;
    tail = nullptr;
}

void init_linkedlist()
{
    reset_linkedlist();
}

size_t get_linkedlist_count() {
    size_t count = 0;
    linkedlist_entry * node = head;
    while (node) {
        count++;
        node = node->next;
    }
    return count;
}

void display_linkedlist_entry() {
    printf("-----------------------------------------\n");
    printf("head: 0x%p\n", head);
    linkedlist_entry * node = head;
    int i = 0;
    while (node) {
        printf(">>>>  entry[%3d] - addr: 0x%p, next: 0x%p, value = %d\n", i, node, node->next, node->value);
        i++;
        node = node->next;
    }
    printf("tail: 0x%p\n", tail);
    printf("-----------------------------------------\n\n");
}

linkedlist_entry * create_linkedlist_entry(int value)
{
    linkedlist_entry * entry = new linkedlist_entry;
    entry->next = nullptr;
    entry->value = value;
    return entry;
}

void append_linkedlist_entry(linkedlist_entry * entry)
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

bool remove_linkedlist_entry(linkedlist_entry * entry)
{
    assert(entry != nullptr);
    if (entry != head) {
		linkedlist_entry * node = head;
		do {
			if (node != nullptr) {
				if (node->next != entry)
					node = node->next;
				else
					break;
			}
			else {
				return false;
			}
		} while (1);

		node->next = entry->next;
		if (node->next == nullptr)
			tail = node;
		delete entry;
		return true;
	}
	else {
        head = head->next;
        if (entry == tail)
            tail = head;
        delete entry;
        return true;
    }
}

bool remove_linkedlist_entry2(linkedlist_entry * entry)
{
    assert(entry != nullptr);
    if (head == nullptr)
        return false;

    if (entry != head) {
		linkedlist_entry * node = head;
		while (node->next != entry) {
			node = node->next;
			if (node == nullptr)
				return false;
		}

		node->next = entry->next;
		if (node->next == nullptr)
			tail = node;
		delete entry;
		return true;
	}
	else {
        head = head->next;
        if (entry == tail)
            tail = head;
        delete entry;
        return true;
    }
}

bool remove_linkedlist_entry_fast(linkedlist_entry * entry)
{
    assert(entry != nullptr);
    linkedlist_entry ** indirect = &head;

    // Notes: *indirect = node->next, except first time value = &head.
    while ((*indirect) != entry) {
        if ((*indirect) != nullptr) {
            indirect = &((*indirect)->next);
        }
        else {
            return false;
        }
    }

    // Notes: node->next = entry->next;
    // if (entry == head && head == tail) {
    if (head == tail) {
        tail = entry->next;
    }
    else if (entry->next == nullptr) {
        tail = (linkedlist_entry *)indirect;      // TODO: tail = node; ???
    }
    (*indirect) = entry->next;
    delete entry;
    return true;
}

void free_linkedlist_entry()
{
    linkedlist_entry * entry;
    linkedlist_entry * node = head;
    while (node) {
        entry = node;
        node = node->next;
        delete entry;
    }

    head = nullptr;
    tail = nullptr;
}

void test_singly_linkedlist_entry()
{
    init_linkedlist_entry();

    linkedlist_entry * node1 = create_linkedlist_entry(1);
    append_linkedlist_entry(node1);
    linkedlist_entry * node2 = create_linkedlist_entry(2);
    append_linkedlist_entry(node2);
    linkedlist_entry * node3 = create_linkedlist_entry(3);
    append_linkedlist_entry(node3);

    printf("append 3 entries;  count = %zu\n", get_linkedlist_count());
    display_linkedlist_entry();

    linkedlist_entry * node = node3;
    remove_linkedlist_entry_fast(node3);

    printf("remove_linkedlist_entry(node3);  count = %zu\n", get_linkedlist_count());
    display_linkedlist_entry();
    
    remove_linkedlist_entry_fast(node1);

    printf("remove_linkedlist_entry(node1);  count = %zu\n", get_linkedlist_count());
    display_linkedlist_entry();
    
    remove_linkedlist_entry_fast(node2);
    
    printf("remove_linkedlist_entry(node2);  count = %zu\n", get_linkedlist_count());
    display_linkedlist_entry();

    free_linkedlist_entry();
    printf("\n");
}

int main(int argc, char * argv[])
{
    test_singly_linkedlist_entry();
#ifdef _WIN32
    ::system("pause");
#endif
    return 0;
}
