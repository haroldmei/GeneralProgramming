/*Given a linked list, return the node where the cycle begins. If there is no cycle, return null.

Note: Do not modify the linked list.

Follow up:
Can you solve it without using extra space?
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
	ListNode *detectCycle(ListNode *head) {

		ListNode *h1 = head;
		ListNode *h2 = head;
		while (h1 && h2){
			h1 = h1->next;
			h2 = h2->next;

			if (h2)
				h2 = h2->next;
			else
				break;

			if (h1 == h2 && h1)
				break;
		}

		if (h1 == h2 && h1)
		{
			set<ListNode*> a;
			for (h1 = head; ; h1 = h1->next){
				set<ListNode*>::iterator itr = a.find(h1);
				if (a.end() != itr)
					return *itr;
				else
					a.insert(h1);
			}
		}
		else
		{
			return 0;
		}
		
	}
};