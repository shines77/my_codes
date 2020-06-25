
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

        ListNode * last, * compare;
        ListNode * iter;

        last = &dummy;
        compare = head;
        iter = head;

        while (iter) {
            ListNode * first = iter;
            iter = iter->next;
            while (iter && iter->val == compare->val) {
                ListNode * temp = iter;
                iter = iter->next;
                delete temp;
            }
            if (iter == compare->next) {
                last = compare;
                compare = iter;
                continue;
            }
            else {
                delete first;
            }
            compare    = iter;
            last->next = iter;
        }
        return dummy.next;
    }
};
