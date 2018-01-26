/*Reverse a linked list from position m to n. Do it in-place and in one-pass.

For example:
Given 1->2->3->4->5->NULL, m = 2 and n = 4,

return 1->4->3->2->5->NULL.

Note:
Given m, n satisfy the following condition:
1 ≤ m ≤ n ≤ length of list.
*/

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
	ListNode* reverseList(ListNode* head, int n) {
		if (!head || !head->next || n <= 0)
			return head;

		int count = 0;
		ListNode* prev = head;
		while (head->next && count++ < n){
			ListNode* tmp = head->next;
			head->next = tmp->next;
			tmp->next = prev;
			prev = tmp;
		}

		head = prev;
		return head;
	}


	ListNode* reverseBetween(ListNode* head, int m, int n) {

		ListNode *hh = head;
		ListNode *hr = 0;
		while (--m > 0){
			if (hh){
				hr = hh;
				hh = hh->next;
			}
			n--;
		}
		hh = reverseList(hh, n - 1);
		if (hr){
			if (hr->next)
				hr->next = hh;
		}
		else
			head = hh;

		return head;
	}
};