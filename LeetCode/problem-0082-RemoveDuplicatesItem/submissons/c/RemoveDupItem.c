
//
// See: https://leetcode.com/submissions/detail/26527734/
//
// Runtime: 5 ms
//

/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     struct ListNode *next;
 * };
 */
struct ListNode* deleteDuplicates(struct ListNode* head) {
    // If list is empty or only have one node, return head directly.
    if (head == NULL || head->next == NULL)
        return head;

    struct ListNode dummy;
    dummy.next = head;

    struct ListNode * last, * comp;
    struct ListNode * iter;

    last = &dummy;
    comp = head;
    iter = head;

    while (iter) {
        struct ListNode * first = iter;
        iter = iter->next;
        while (iter && comp->val == iter->val) {
            //ListNode * temp = iter;
            iter = iter->next;
            //delete temp;
        }
        if (iter == comp->next) {
            last = comp;
            comp = iter;
            continue;
        }
        else {
            //delete first;
        }
        comp       = iter;
        last->next = iter;
    }
    return dummy.next;
}
