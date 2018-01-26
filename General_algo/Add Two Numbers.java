/*You are given two non-empty linked lists representing two non-negative integers. The digits are stored in reverse order and each of their nodes contain a single digit. Add the two numbers and return it as a linked list.

You may assume the two numbers do not contain any leading zero, except the number 0 itself.

Example

Input: (2 -> 4 -> 3) + (5 -> 6 -> 4)
Output: 7 -> 0 -> 8
Explanation: 342 + 465 = 807.
*/

/**
 * Definition for singly-linked list.
 * public class ListNode {
 *     int val;
 *     ListNode next;
 *     ListNode(int x) { val = x; }
 * }
 */
public class Solution {
	
    public ListNode addTwoNumbers(ListNode l1, ListNode l2) {
		int carry = 0;
		
		ListNode result = l1;
		ListNode lastNode = l1;
		do{
			if (l1 != null && l2 != null)
			{
				l1.val += l2.val + carry;
				carry = l1.val / 10;
				l1.val %= 10;
				lastNode = l1;
				l1 = l1.next;
				l2 = l2.next;
			}
			else if (l2 != null)
			{
				l2.val += carry;
				carry = l2.val / 10;
				l2.val %= 10;
				
				lastNode.next = l2;
				lastNode = l2;
				l2 = l2.next;
			}
			else if (l1 != null)
			{
				l1.val += carry;
				carry = l1.val / 10;
				l1.val %= 10;
				
				lastNode = l1;
				l1 = l1.next;
			}
			else
			{
				if (carry > 0)
				{
					ListNode carryNode = new ListNode(carry);
					lastNode.next = carryNode;
				}
				break;
			}
		}while(true);
		
		return result;
    }
}