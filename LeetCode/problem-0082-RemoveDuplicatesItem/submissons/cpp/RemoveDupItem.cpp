
//
// See: https://leetcode.com/submissions/detail/26527623/
//
// Runtime: 11 ms
//

/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     ListNode *next;
 *     ListNode(int x) : val(x), next(NULL) {}
 * };
 */
class Solution {
public:
    ListNode * deleteDuplicates(ListNode * head) {
        ListNode dummy(0);
        dummy.next = head;

        ListNode * last, * comp;
        ListNode * iter;

        last = &dummy;
        comp = head;
        iter = head;

        while (iter) {
            ListNode * first = iter;
            iter = iter->next;
            while (iter && comp->val == iter->val) {
                ListNode * temp = iter;
                iter = iter->next;
                delete temp;
            }
            if (iter == comp->next) {
                last = comp;
                comp = iter;
                continue;
            }
            else {
                delete first;
            }
            comp       = iter;
            last->next = iter;
        }
        return dummy.next;
    }
};
