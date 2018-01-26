/*Reverse a singly linked list.

click to show more hints.

Hint:
A linked list can be reversed either iteratively or recursively. Could you implement both?
*/Reverse Linked List

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
    ListNode* reverseList(ListNode* head) {
		if (!head || !head->next)
			return head;

        ListNode* prev = head;
		while (head->next){
			ListNode* tmp = head->next;
			head->next = tmp->next;
			tmp->next = prev;
			prev = tmp;
		}

		head = prev;
		return head;
    }
};